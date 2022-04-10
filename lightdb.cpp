#include "util/dir_util.h"
#include "lightdb.h"
#include "sync/waitGroup.h"
#include "sync/CTimer.h"

namespace lightdb{

LightDB::LightDB(){}

void runMerge(LightDB* lightdb){
    printf("trying to merge ---\n");
    lightdb->Merge();
}

Status LightDB::Open(Config* config, bool merge){
    Status s;

    this->config = config;
    CreateDir(config->dirPath.c_str());

    //load the db files from disk
    FileIds activeFileIds;
    s = Build(config->dirPath, config->rWMethod, config->blockSize, archivedFiles, activeFileIds);
    if(!s.ok()){
        return s;
    }

    //set active files for writing 每种不同类型的value都有一个活跃数据文件
    for(auto it = activeFileIds.begin(); it!=activeFileIds.end(); it++){
        // it->first是数据类型的型号（0-4），it->second是文件Id，注意是Id，即DBFile的Id，不是fileid
        DBFile* af = new DBFile(config->dirPath, it->second, config->rWMethod, config->blockSize, it->first);
        activeFiles.insert(std::make_pair(it->first, af));
    }

    // expires是每个key的过期时间，同时包含了每个key的所属类型（0-4）
    for(int i = 0; i < DataStructureNum; i++){
        expires.insert(std::make_pair(uint16_t(i), std::unordered_map<std::string, uint64_t>()));
    }

    cache = new LRUCache(config->cacheCapacity);

    // 遍历所有类型的db文件，建立索引，每种类型都有相应的存储索引信息的结构
    s = this->loadIdxFromFiles();
    if(!s.ok()){
        return s;
    }
    printf("checkInterval :%d \n", config->mergeCheckInterval);
    if(merge) {
        CTimer *cTimer = new CTimer();
        std::thread mergeThread(
                [&](LightDB *lightDb, int mergeCheckInterval, CTimer *cTimer) {
                    printf("check interval1 :%d \n", mergeCheckInterval);
                    cTimer->AsyncLoop(mergeCheckInterval, runMerge, lightDb);
                }, this, this->config->mergeCheckInterval, cTimer);
        mergeThread.detach();
        printf("定时器设置完毕\n");
    }
    return Status::OK();
}

//check if the key or value exceed the length limitation
Status LightDB::CheckKeyValue(string key, string value){
    int keyLength = key.size();
    if(keyLength == 0){
        return Status::EmptyKeyError();
    }
    if(keyLength > config->maxKeySize){
        return Status::KeyTooLargeErr(key);
    }
    if(value.size() > config->maxValueSize){
        return Status::ValueTooLargeErr(value);
    }
    return Status::OK();
}

//check if the key or values exceed the length limitation
Status LightDB::CheckKeyValue(string key, vector<string> values){
    int keyLength = key.size();
    if(keyLength == 0){
        return Status::EmptyKeyError();
    }
    if(keyLength > config->maxKeySize){
        return Status::KeyTooLargeErr(key);
    }
    for(auto it = values.begin(); it!=values.end(); it++){
        if(it->size() > config->maxValueSize){
            return Status::ValueTooLargeErr(*it);
        }
    }
    return Status::OK();
}


DBFile* LightDB::getActiveFile(DataType dataType) {
    return activeFiles[dataType];
}

// if the key is exist and expired, return true and erase indexer and expires element, else return false
bool LightDB::CheckExpired(string key, DataType dataType){
    if(expires[dataType].find(key) == expires[dataType].end()){
        return false;
    }
    uint64_t deadline = expires[dataType][key];
    uint64_t currentTimeStamp = getCurrentTimeStamp();
    if(deadline < currentTimeStamp){
        Entry* e;
        switch (dataType) {
            case String:
                e = Entry::NewEntryNoExtra(key, "", String, StringRem);
                strIdx.indexes->erase(key);
                break;
            case List:
                e = Entry::NewEntryNoExtra(key, "", List, ListLClear);
                listIdx.indexes->LClear(key);
                break;
            case Hash:
                e = Entry::NewEntryNoExtra(key, "", Hash, HashHClear);
                hashIdx.indexes->HClear(key);
                break;
            case Set:
                e = Entry::NewEntryNoExtra(key, "", Set, SetSClear);
                setIdx.indexes->SClear(key);
                break;
            case ZSet:
                e = Entry::NewEntryNoExtra(key, "", ZSet, ZSetZClear);
                sortedSetIdx.indexes->ZClear(key);
                break;
            default:
                break;
        }
        // 删除操作即为追加一条entry，只不过value为空，这样就意味着要删除这个key对应的所有entry
        store(e);
        expires[dataType].erase(key);
        return true;
    }
    return false;
}

//store the entry into activeFile
Status LightDB::store(Entry* entry){
    Status s;
    DBFile* activeFile = this->getActiveFile(static_cast<DataType>(entry->GetType()));
    if(activeFile->WriteOffset > config->blockSize) {
        DBFile* newActiveFile = new DBFile(config->dirPath, activeFile->Id + 1, config->rWMethod, config->blockSize, entry->GetType());
        this->activeFiles[entry->GetType()] = newActiveFile;
        this->archivedFiles[entry->GetType()].insert(std::make_pair(activeFile->Id, activeFile));
        activeFile = newActiveFile;
    }
    s = activeFile->Write(entry);
    if(!s.ok()){
        return s;
    }
    return Status::OK();
}

Status LightDB::Merge(){
    printf("trying to merge---\n");
    if(isMerging){
        return Status::IsMergingErr();
    }
    isMerging = true;

    // create a temporary directory for storing the new db files.
    std::string mergePath = config->dirPath + "/"  +mergeDirName + "/";
    CreateDir(mergePath.c_str());
    dump();
    //todo 先串行测试string
//    std::thread mergeStringThread(
//            [](LightDB* p){
//                p->mergeString();
//            },
//            this);
//    mergeStringThread.join();

    this->mergeString();

    //sleep(20);
    printf("all dump end \n");
    isMerging = false;
    return Status::OK();
}

// 各个value类型中，对应的entry都是保存std::string key和std::string value，而不会保存list、hash等。例如value类型为list时，每个key都会对应一个list，这个list中的所有string都会和key分别组成一对，存放到一个entry中，而不会把key和对应的整个list存放到一个entry中
void LightDB::dump(){
    std::string path = config->dirPath + "/" + mergeDirName;
    std::thread* dumpListThread;
    std::thread* dumpHashThread;
    std::thread* dumpSetThread;
    std::thread* dumpZSetThread;
    for(int idx = List; idx < DataStructureNum; idx++){
        //printf("lalala \n");
        switch (idx){
            case List: {
                dumpListThread = new std::thread([](LightDB *p, string path) {
                    printf("p == null :%d \n", p == nullptr);
                    p->dumpInternal(path, List); }, this, path);
                //先串行
                //todo debug
                //dumpInternal(wg, path, List);
                break;
            }
            case Hash: {
                //todo debug 先串行
                dumpHashThread = new std::thread ([](LightDB *p, string path) {
                    printf("p == null :%d \n", p == nullptr);
                    p->dumpInternal(path, Hash); }, this, path);
                //dumpInternal(wg, path, Hash);
                break;
            }
            case Set: {
                //todo debug 先串行
                dumpSetThread = new std::thread ([](LightDB *p, string path) {
                    printf("p == null :%d \n", p == nullptr);
                    p->dumpInternal(path, Set); }, this, path);
                //dumpInternal(wg, path, Set);
                break;
            }
            case ZSet:
                //todo debug 先串行
                dumpZSetThread = new std::thread ([](LightDB* p, string path){
                    printf("p == null :%d \n", p == nullptr);
                    p->dumpInternal(path, ZSet);}, this, path);
                //dumpInternal(wg, path, ZSet);
                break;
        }
    }
    dumpListThread->join();
    dumpHashThread->join();
    dumpSetThread->join();
    dumpZSetThread->join();
    delete dumpListThread;
    delete dumpHashThread;
    delete dumpSetThread;
    delete dumpZSetThread;
    printf("Main threading \n");
    /* wg.Wait(); */
    return;
}

void LightDB::dumpInternal(std::string path, DataType eType){
    printf("begin dump ! type:%d \n", eType);
    Status s;
    //todo 先去除mergeThreshold Check
    //if(archivedFiles[eType].size() + 1 < config->mergeThreshold){
    //    return;
    //}

    DBFile* file = new DBFile(path, 0, config->rWMethod, config->blockSize, eType);
    std::vector<DBFile*> mergeFiles;
    mergeFiles.push_back(file);
    switch (eType) {
        case List:
            s = dumpList(mergeFiles, path);
            break;
        case Hash:
            s = dumpHash(mergeFiles, path);
            break;
        case Set:
            s = dumpSet(mergeFiles, path);
            break;
        case ZSet:
            s = dumpZSet(mergeFiles, path);
            break;
        default:
            break;
    }
    if(!s.ok()){
        printf("dump err,code: %d\n", s.Code());
    }

    //remove old archive files
    for(auto it : this->archivedFiles[eType] ){
        std::remove( (config->dirPath + "/" + it.second->FileName()).c_str());
    }

    //remove old active file
    std::remove( (config->dirPath + "/" + activeFiles[eType]->FileName()).c_str() );

    //rename merge files, copy merge file to dir path
    for(auto mgFile : mergeFiles){
        rename( (path + "/" + mgFile->FileName()).c_str(), (config->dirPath + "/" + mgFile->FileName()).c_str());
    }

    //printf("dump end, type:%d \n", eType);
}



Status LightDB::dumpStore(vector<DBFile*>& mergeFiles, std::string mergePath, Entry* e){
    //get the last file of the type
    Status s;
    DBFile* df = mergeFiles[mergeFiles.size()-1];
    if(df->WriteOffset > config->blockSize){
        //free the last file
        /* delete(df); */
        df = new DBFile(mergePath, df->Id+1, config->rWMethod, config->blockSize, e->GetType());
        mergeFiles.push_back(df);
    }
    s = df->Write(e);
    return s;
}




void LightDB::mergeString(){

    //todo 先关闭mergeThreshold Check
//    if(archivedFiles[String].size() < config->mergeThreshold){
//        return;
//    }

    std::string mergePath = config->dirPath + "/" + mergeDirName;
    ArchivedFiles mergeFiles;
    FileIds tmp;
    Status s = BuildType(mergePath, config->rWMethod, config->blockSize, mergeFiles, tmp, String);
    if(!s.ok()){
        printf("[mergeString] build db file err \n");
        s.Report();
    }

    std::unordered_map<uint32_t, DBFile*> archFiles = mergeFiles[String];
    int maxFileId = 0;
    for(auto it = archFiles.begin(); it!=archFiles.end(); it++){
        if(it->first > maxFileId){
            maxFileId = it->first;
        }
    }

    vector<int> fileIds;
    //skip the merged Files;
    printf("str File no %d \n", archivedFiles[String].size());
    int size = archivedFiles[String].size();

    int archFilesSize = archFiles.size();
    // 从不活跃数据文件中选出未进行merge的文件，对它们进行merge。临时目录中可能存在一些merged db文件，这可能是因为上次数据库进行merge时，发生宕机了，就会有一些merged db文件残留在临时目录中，没来得及移出临时目录，这些残留的merged db文件不应参与到这次merge。这次merge完成后，新的merged db文件会和这些残留的上次的merged db文件一起移动到父目录，临时目录就会被清空了
    for(auto it = archivedFiles[String].begin(); it!=archivedFiles[String].end(); it++){
        if(it->first >= maxFileId){
            fileIds.push_back(it->first);   
        }
    }

    // 从小到大遍历不活跃的数据文件，对其进行merge
    sort(fileIds.begin(), fileIds.end());

    //merge的时候需要建立新的索引；
    StrSkiplist* newStrSkipList = new StrSkiplist();

    for(int i = 0; i<fileIds.size(); i++){
        DBFile* dbFile = archivedFiles[String][fileIds[i]];
        vector<Entry*> validEntries;
        s = FindValidEntries(validEntries, dbFile);

        //rewrite the valid entries
        DBFile* rewriteDF = nullptr;
        int rewriteFileId = 0;
        for(auto entry : validEntries){
            if(rewriteDF == nullptr || rewriteDF->WriteOffset > config->blockSize){
                rewriteDF = new DBFile(mergePath, rewriteFileId, config->rWMethod, config->blockSize, String);
                this->archivedFiles[String][rewriteFileId] = rewriteDF;
                archFiles.insert(std::make_pair(rewriteFileId, rewriteDF));
                rewriteFileId += 1;
            }
            s = rewriteDF->Write(entry);
            if(!s.ok()){
                printf("rewrite entry err! \n");
                return;
            }

            //update index
            Indexer idx;
            //if(this->strIdx->indexes->get(entry->meta->key, idx)){
            idx.fileId = rewriteFileId;
            idx.offset = rewriteDF->WriteOffset - entry->Size();
            if(config->indexMode == KeyValueMemMode){
                idx.meta = new Meta();
                idx.allocated = true;
                idx.meta->value = entry->meta->value;
            }
            newStrSkipList->put(entry->meta->key, idx);
            //}
        }
        //delete older archivedFiles
        std::string oldfilename = dbFile->FileName();
        delete(dbFile);
        std::remove((config->dirPath + "/" + oldfilename).c_str());
    }


    // 把merge出的db文件移出临时目录，移到父目录中，这样临时目录就是空的了
    this->strIdx.mtx.lock();//移动文件的时候需要阻塞
    for(auto file : archFiles){
        char buf[50];
        sprintf(buf, "%09d.data.str", file.first);
        std::string name;
        name.assign(buf);
        if(rename((mergePath + "/" + name).c_str(), (config->dirPath + "/" + name).c_str()) < 0){
            printf("rename error\n");
        }
    }
    //reload dbFiles;
    //this->loadDBFiles(String);

    //merge完成之后把新的索引设置到lightdb对象
    this->strIdx.indexes = newStrSkipList;
    //把新的索引更新到旧索引中
    for(newStrSkipList->Begin(); !newStrSkipList->End(); newStrSkipList->Next()){
        std::string key;
        Indexer value;
        newStrSkipList->CurIterKeyValue(key, value);
        strIdx.indexes->put(key, value);
    }
    this->strIdx.mtx.unlock(); //新索引建立完成，可以解锁了
    return;
}



// 只有mergestring时使用这个函数
Status LightDB::FindValidEntries(std::vector<Entry*>& entries, DBFile* df){
    Status s;
    uint64_t offset = 0;
    while (offset < df->WriteOffset) {
        //printf("offset ------:%lu \n", offset);
        Entry* e = new Entry();
        s = df->Read(offset, *e);
        if(!s.ok()){
            if(s.Code() == kEndOfFile){
                return Status::OK();
            }
            return s;
        }
        if(validEntry(e, offset, df->Id)){
            //printf("valid entry !!!! \n");
            entries.push_back(e);
        }

        offset += e->Size();
    }
    return s;
}

// 只有mergestring时使用这个函数，fileid和offset是entry所在文件的Id和在这个文件中的偏移量
bool LightDB::validEntry(Entry* e, int64_t offset, uint32_t fileId){
    if(e == nullptr){
        return false;
    }

    if(expires[String].find(e->meta->key) != expires[String].end() &&
    expires[String][e->meta->key] < getCurrentTimeStamp()){
        return false;
    }

    if(e->GetMark() == StringSet || e->GetMark() == StringExpire){
        Indexer indexer;
        if(e->meta->value == "value-352931"){
            printf("出事了\n");
        }
        bool get = strIdx.indexes->get(e->meta->key, indexer);
        if(!get){
            return false;
        }
        if(indexer.offset == offset && indexer.fileId == fileId){
                // 每个key对应的indexer包含了这个key最新的value，所以只要entry->offset和对应的indexer->offset相等，就能说明这个entry是最新的，那么这个entry就有效
                return true;
        }
    }

    return false;
}


// 为db的不活跃数据文件建立DBFile实例，并获取db的活跃数据文件的Id号
Status LightDB::loadDBFiles(DataType dataType){
    Status s;
    ArchivedFiles archFiles;
    FileIds activeFileIds;
    s = BuildType(config->dirPath, config->rWMethod, config->blockSize, archFiles, activeFileIds, dataType);
    if(!s.ok()){
        return s;
    }
    this->archivedFiles[dataType] = archFiles[dataType];
    this->activeFiles[dataType] = new DBFile(config->dirPath, activeFileIds[dataType], config->rWMethod, config->blockSize, dataType);
    return Status::OK();
}


}
