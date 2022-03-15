

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

    DBFile::DBFile(std::string path, uint32_t fileId, FileRWMethod method, uint32_t blockSize, uint16_t eType){
        char buf[20];
        memset(buf, '\0', sizeof(buf));
        sprintf(buf, DBFileFormatNames[eType].c_str(), fileId);
        std::string fileName;
        fileName.assign(buf);
        _fileName = fileName;

        std::string filePath = path + "/" + fileName;
        //cout<< filePath <<endl;
        _fd = open(filePath.c_str(), O_RDWR|O_APPEND|O_CREAT);
        if( _fd < 0){
            printf("open file failed: %d \n", errno);
            exit (errno);
        }
        WriteOffset = lseek(_fd, 0l, SEEK_END);
        lseek(_fd, 0l, SEEK_SET);
        printf("WriteOffset :%d \n", WriteOffset );
        if(WriteOffset == -1){
            printf("error occurred : errno :%d \n", errno);
            exit(-1);
        }
    }

    DBFile::~DBFile(){
        int ret = close(_fd);
        if( ret < 0 ){
            printf("close fd err: ret :%d \n", ret);
        }
    }

    std::string DBFile::FileName(){
        return _fileName;
    }

    Status DBFile::Read(uint64_t offset, Entry& entry){
        Status s;
        lseek(_fd,offset,SEEK_SET);
        s = Read(entry);
        return s;
    }

    Status DBFile::Read(Entry& entry){
        std::string buf;
        Status status = ReadBuf(buf, EntryHeaderSize);
        //printf("fileName %s\n", this->_fileName.c_str());
//        for(int i = 0; i < buf.size(); i++){
//            std::cout << "0x" <<  std::hex << static_cast<unsigned short>(buf.c_str()[i]) << " " ;//输出十六进制FF
//        }

        if(!status.ok()){
            return status;
        }
        entry.Decode(buf);
        std::string key;
        if(entry.meta->keySize>0){
            status = ReadBuf(key, entry.meta->keySize);
            if(!status.ok() && status.Code()!=kEndOfFile){
                printf("code:  %d", status.Code());
                return status;
            }
        }
        entry.meta->key = key;
        std::string value;
        if(entry.meta->valueSize > 0){
            status = ReadBuf(value, entry.meta->valueSize);
            if(!status.ok()){
                return status;
            }
        }
        entry.meta->value = value;
        std::string extra;
        if(entry.meta->extraSize>0){
            status = ReadBuf(extra, entry.meta->extraSize);
            if(!status.ok()){
                return status;
            }
        }
        entry.meta->extra = extra;
        return Status::OK();
    }

    Status DBFile::ReadBuf(std::string& result, uint32_t size){
        char buf[size];
        memset(buf, 0, size);
        Status status;
        //size_t read_size = read(_fd, buf, size);
        //printf("offset %d \n", this->Offset);
        //printf("size :%d \n", size);
        while (true) {
            //printf("point a \n");
            ssize_t read_size = read(_fd, buf, size);
//            for(int i = 0; i < sizeof(buf); i++){
//                std::cout << "0x" <<  std::hex << static_cast<unsigned short>(buf[i]) << " " ;//输出十六进制FF
//            }

            if (read_size < 0) {  // Read error.
                printf("errrrrrr----\n");
                if (errno == EINTR) {
                    continue;  // Retry
                }
                status = PosixError(_fileName, errno);
                break;
            }else if(read_size == 0 && size!=0){
                //end of file
                printf("end of file \n");
                return Status::EndOfFile(this->_fileName);
            }else{
                //printf("add offset\n");
                this->Offset += read_size;
            }
            result.assign(buf, size);
            this->Offset+=size;
            break;
        }
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
            ssize_t write_size = write(_fd, data, size);
            //printf("write_size %d \n", write_size);
            if(write_size < 0){
                if(errno == EINTR){
                    continue;
                }
                status = PosixError(_fileName, errno);
            }else{
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

    bool DBFile::Close(){
        if(close(this->_fd)){
            Sync();
            return true;
        }
        return false;
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

    Status buildInternal(std::string path, FileRWMethod method, int64_t blockSize, uint16_t eType, ArchivedFiles& archiveFiles, FileIds& activeFileIds){
        DIR *dir;
        struct dirent *ptr;
        char base[1000];
        std::unordered_map<uint16_t,std::vector<int>> fileIdsMap;
        if((dir = opendir(path.c_str())) == NULL){
            perror("Open dir error...");
            exit(1);
        }
        while((ptr=readdir(dir)) != NULL){
            if(std::string(ptr->d_name).rfind(".swp")!=-1){
                continue;
            }
            if(std::string(ptr->d_name).rfind(".swo")!=-1){
                continue;
            }
            if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
                continue;
            else if(ptr->d_type == 8){
                std::vector<std::string> splitNames = splitWithStl(std::string(ptr->d_name), ".");
                bool fileid_valid;
                int id = lightdb::stringToInt(splitNames[0], fileid_valid);

                uint16_t type_16 = DBFileSuffixName[splitNames[2]];
                DataType type = static_cast<DataType>(type_16);
                if(type == eType || eType == All){
                    fileIdsMap[type].push_back( id );
                }
            }else if(ptr->d_type == 4){
                //todo
            }
        }
//        for(auto it = fileIdsMap.begin(); it!=fileIdsMap.end(); it++){
//            printf("type%d , size:%d \n", it->first, it->second.size());
//        }
        printf("start build---\n");

        uint16_t typ = 0;
        for(; typ<5; typ++){
            std::unordered_map<uint32_t, DBFile*> files;
            std::vector<int> fileIds = fileIdsMap[typ];
            uint32_t activeFileId = 0;

            if(fileIds.size() > 0){
                activeFileId = fileIdsMap[typ].size() - 1;
//                printf("size:::%d , activeFileId:%d \n", fileIdsMap[typ].size(), activeFileId);

                std::sort(fileIds.begin(), fileIds.end());
                for(int idx = 0; idx<fileIds.size(); idx++){
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
            perror("Open dir error...");
            exit(1);
        }

        while ((ptr=readdir(dir)) != NULL)
        {
            if(std::string(ptr->d_name).rfind(".swp")!=-1){
                continue;
            }
            if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
                continue;
            else if(ptr->d_type == 8)    ///file
            {
                //printf("d_name:%s/%s\n",basePath,ptr->d_name);
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