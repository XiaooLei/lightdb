

#include "db_file.h"
#include <stdio.h>
#include <iostream>

namespace lightdb{

    const std::unordered_map<std::string,uint16_t> DBFileSuffixName = {
            {"str",0},
            {"list",1},
            {"hash",2},
            {"set",3},
            {"zset",4}
    };

    std::unordered_map<uint16_t, std::string> DBFileFormatNames = {
            {0, "%09d.data.str"},
            {1, "%09d.data.list"},
            {2, "%09d.data.hash"},
            {3, "%09d.data.set"},
            {4, "%09d.data.zset"},
    };

    DBFile::DBFile(std::string path, uint32_t fileId, FileRWMethod method, uint32_t blockSize, uint16_t eType):Id(fileId),_fileName(""){
        char buf[20];
        memset(buf, '\0', sizeof(buf));
        sprintf(buf, DBFileFormatNames[eType].c_str(), fileId);
        std::string fileName;
        fileName.assign(buf);
        _fileName = fileName;

        std::string filePath = path + "/" + fileName;

        _fd = open(filePath.c_str(), O_RDWR|O_APPEND|O_CREAT);
        if( _fd < 0){
            printf("file:%s ", filePath.c_str());
            perror("open file failed, the reason is");
            exit (errno);
        }

        WriteOffset = lseek(_fd, 0l, SEEK_END);
        // 当open时，如果使用了O_APPEND，那么无论lseek把文件指针指向何处，当write时，都会从文件的结尾处写入
        lseek(_fd, 0l, SEEK_SET);
        if(WriteOffset == -1){
            printf("error occurred : errno :%d \n", errno);
            exit(-1);
        }
    }

    DBFile::~DBFile(){
        syncfs(this->_fd);
        int ret = close(_fd);
        if( ret < 0 ){
            printf("close fd err: ret :%d \n", ret);
        }
    }

    std::string DBFile::FileName(){
        return _fileName;
    }

    Status DBFile::Read(uint64_t offset, Entry& entry){
        std::string buf;
        Status status = ReadBuf(buf, offset, EntryHeaderSize);
        if(!status.ok()){
            return status;
        }
        entry.Decode(buf);
        offset += EntryHeaderSize;

        if(entry.meta->keySize>0){
            status = ReadBuf(entry.meta->key, offset, entry.meta->keySize);
            if(!status.ok() && status.Code()!=kEndOfFile){
                return status;
            }
        }
        offset += entry.meta->keySize;

        if(entry.meta->valueSize > 0){
            status = ReadBuf(entry.meta->value, offset, entry.meta->valueSize);
            if(!status.ok()){
                return status;
            }
        }
        offset += entry.meta->valueSize;

        if(entry.meta->extraSize>0){
            status = ReadBuf(entry.meta->extra, offset, entry.meta->extraSize);
            if(!status.ok()){
                return status;
            }
        }
        offset += entry.meta->extraSize;

        uint32_t crc = CRC_Compute(entry.meta->value);
        if(crc != entry.crc32){
            return Status::InvalidCrcErr(entry.meta->key);
        }
        return Status::OK();
    }

    Status DBFile::ReadBuf(std::string& result, uint64_t offset, uint32_t size){
        char buf[size];
        memset(buf, 0, size);
        Status status;
        uint32_t nleft = size;
        ssize_t nread = 0;
        ssize_t nreadsum = 0;
        while (nleft > 0) {
            if ((nread = pread(_fd, buf + nreadsum, nleft, offset)) < 0) {  // Read error.
                if (errno == EINTR) {
                    nread = 0;
                } else {
                    status = PosixError(_fileName, errno);
                    break;
                }
            }else if(nread == 0){
                //printf("end of file \n");
                return Status::EndOfFile(this->_fileName);
            }
            nleft -= nread;
            offset += nread;
            nreadsum += nread;
        }
        result.assign(buf, size);
        this->Offset += size;
        return status;
    }

    Status DBFile::Write(Entry* entry){
        std::string buf;
        entry->Encode(buf);
        char* data = static_cast<char *>(malloc(buf.size()));
        memcpy(data, buf.c_str(), buf.size());

        Status status;
        uint32_t size = buf.size();
        while(size > 0){
            // 因为文件是以追加方式打开的，所以数据只可能追加到文件末尾
            ssize_t write_size = write(_fd, data, size);
            if(write_size < 0){
                if(errno == EINTR){
                    write_size = 0;
                } else {
                    status = PosixError(_fileName, errno);
                    break;
                }
            }
            else {
                WriteOffset += write_size;
            }
            data += write_size;
            size -= write_size;
        }
        return status;
    }

    void DBFile::SetOffset(int64_t offset){
        this->Offset = offset;
    }

    void DBFile::Sync(){
        syncfs(this->_fd);
    }

    Status BuildType(std::string path, FileRWMethod method, int64_t blockSize, ArchivedFiles& archivedFiles, FileIds fileIds, DataType type){
        return buildInternal(path, method, blockSize, type, archivedFiles, fileIds);
    }

    Status Build(std::string path, FileRWMethod method, int64_t blockSize, ArchivedFiles& archivedFiles, FileIds& fileIds){
        return buildInternal(path, method, blockSize, All, archivedFiles, fileIds);
    }

    // 读取指定目录下的所有数据文件，为不活跃数据文件建立DBFile实例，并获取活跃数据文件的Id号
    Status buildInternal(std::string path, FileRWMethod method, int64_t blockSize, uint16_t eType, ArchivedFiles& archiveFiles, FileIds& activeFileIds){
        DIR *dir;
        struct dirent *ptr;
        char base[1000];
        // 每种类型包含的文件id，注意不是fileid
        std::unordered_map<uint16_t,std::vector<int>> fileIdsMap;
        if((dir = opendir(path.c_str())) == NULL){
            perror("Open dir error...");
            exit(1);
        }
        while((ptr=readdir(dir)) != NULL){
            // .swp和.swo文件都是临时交换文件，应该被忽略掉
            if(std::string(ptr->d_name).rfind(".swp")!=-1){
                continue;
            }
            if(std::string(ptr->d_name).rfind(".swo")!=-1){
                continue;
            }
            if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
                continue;
            else if(ptr->d_type == 8){
                // 普通文件，将文件名按"."分割，例如000000000.data.set被分割为000000000、data、set
                std::vector<std::string> splitNames = splitWithStl(std::string(ptr->d_name), ".");
                bool fileid_valid;
                int id = lightdb::stringToInt(splitNames[0], fileid_valid);

                uint16_t type_16 = DBFileSuffixName[splitNames[2]];
                DataType type = static_cast<DataType>(type_16);
                if(type == eType || eType == All){
                    fileIdsMap[type].push_back(id);
                }
            } else if(ptr->d_type == 4 || std::string(ptr->d_name).rfind(mergeDirName)){
                // 目录文件 Todo
                // 如果是mergeDir，那么就要
                //
            }
        }

        //printf("start build---\n");

        for(uint16_t typ = 0; typ<5; typ++){
            if(eType!=All && typ != eType){
                continue;
            }
            std::unordered_map<uint32_t, DBFile*> files;
            std::vector<int> fileIds = fileIdsMap[typ];
            uint32_t activeFileId = 0;
            std::sort(fileIds.begin(), fileIds.end());

            if(fileIds.size() > 0){

//                printf("size:::%d , activeFileId:%d \n", fileIdsMap[typ].size(), activeFileId);

                activeFileId = fileIds.back();

                for(int idx = 0; idx < fileIds.size() - 1; idx++){
                    int id = fileIds[idx];
                    DBFile* db_file = new DBFile(path, uint32_t(id), method, blockSize, typ);
                    files.insert(std::make_pair(id, db_file));
                }
            }
            activeFileIds.insert(std::make_pair(typ, activeFileId));
            archiveFiles.insert(std::make_pair(typ, files));
        }
        return Status::OK();
    }

    std::vector<std::string> readFileList(char *basePath)
    {
        std::vector<std::string> result;
        DIR *dir;
        struct dirent *ptr;
        char base[1000];

        if ((dir=opendir(basePath)) == NULL)
        {
            perror("Open dir error..., the reason is");
            exit(1);
        }

        while ((ptr=readdir(dir)) != NULL)
        {
            // ptr->d_name是文件名，不包含目录
            if(std::string(ptr->d_name).rfind(".swp")!=-1 || std::string(ptr->d_name).rfind(".swo")!=-1){
                continue;
            }

            if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
                continue;
            else if(ptr->d_type == 8)    ///file
            {
                result.push_back(std::string(ptr->d_name));
            }
            else if(ptr->d_type == 10)    ///link file
            {
                //printf("d_name:%s/%s\n",basePath,ptr->d_name);
                result.push_back(std::string(ptr->d_name));
            }
            else if(ptr->d_type == 4)    ///dir
            {
                memset(base,'\0',sizeof(base));
                strcpy(base,basePath);
                strcat(base,"/");
                strcat(base,ptr->d_name);
                result.push_back(std::string(ptr->d_name));
                readFileList(base);
            }
        }
        closedir(dir);
        return result;
    }




}
