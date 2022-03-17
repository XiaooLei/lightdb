#include "util/dir_util.h"
#include "lightdb.h"

namespace lightdb{

LightDB::LightDB(){
}

Status LightDB::Open(Config* config){
    Status s;

    this->config = config;
    CreateDir(config->dirPath.c_str());

    //load the db files from disk
    FileIds activeFileIds;
    s = Build(config->dirPath, config->rWMethod, config->blockSize, archivedFiles, activeFileIds);
    if(!s.ok()){
        return s;
    }

    //set active files for writing
    for(auto it = activeFileIds.begin(); it!=activeFileIds.end(); it++){
        DBFile* af = new DBFile(config->dirPath, it->second, config->rWMethod, config->blockSize, it->first);
        activeFiles.insert(std::make_pair(it->first, af));
    }

    for(int i = 0; i<DataStructureNum; i++){
        expires.insert(std::make_pair(uint16_t(i), std::unordered_map<std::string, uint64_t>()));
    }

    cache = new LRUCache(config->cacheCapacity);

    s = this->loadIdxFromFiles();
    if(!s.ok()){
        return s;
    }
    return Status::OK();
}

//check if the key and values exceed the  length limitation
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

//check if the key and values exceed the  length limitation
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

//check if the key is expired, if expired or the key specified does not exist,return true, else return false
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
            case List:
                e = Entry::NewEntryNoExtra(key, "", List, ListLClear);
                listIdx.indexes->LClear(key);
            case Hash:
                e = Entry::NewEntryNoExtra(key, "", Hash, HashHClear);
                hashIdx.indexes->HClear(key);
            case Set:
                e = Entry::NewEntryNoExtra(key, "", Set, SetSClear);
                setIdx.indexes->SClear(key);
            case ZSet:
                e = Entry::NewEntryNoExtra(key, "", ZSet, ZSetZClear);
                sortedSetIdx.indexes->ZClear(key);
        }
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
    s = activeFile->Write(entry);
    if(!s.ok()){
        return s;
    }
    return Status::OK();
}

Status LightDB::Merge(){
    if(isMerging){
        return Status::IsMergingErr();
    }
    isMerging = true;

    // create a temporary directory for storing the new db files.
    std::string mergePath = config->dirPath + "/"  +mergeDirName + "/";
    CreateDir(mergePath.c_str());
    WaitGroup wg;
    wg.Add( DataStructureNum - 4);

    dump(wg);
    //todo
    mergeString(wg);
    wg.Wait();
    printf("all dump end \n");
    isMerging = false;
    return Status::OK();
}

void LightDB::dump(WaitGroup& wg){
    std::string path = config->dirPath + "/" + mergeDirName;
    for(int idx = List; idx < DataStructureNum; idx++){
        //printf("lalala \n");
        switch (idx){
            case List: {
                std::thread dumpThread([&](LightDB *p) { p->dumpInternal(wg, path, List); }, this);
                dumpThread.detach();
                //先串行
                //todo
                //dumpInternal(wg, path, List);
                break;
            }
            case Hash: {
                std::thread dumpThread([&](LightDB *p) { p->dumpInternal(wg, path, Hash); }, this);
                dumpThread.detach();
                //dumpInternal(wg, path, Hash);
                break;
            }
            case Set: {
                std::thread dumpThread([&](LightDB *p) { p->dumpInternal(wg, path, Set); }, this);
                dumpThread.detach();
                //dumpInternal(wg, path, Set);
                break;
            }
            case ZSet:
                std::thread dumpThread([&](LightDB* p){p->dumpInternal(wg, path, ZSet);}, this);
                //dumpInternal(wg, path, ZSet);
                dumpThread.detach();
                break;
        }
    }
    printf("Main threading \n");
    wg.Wait();
    return;
}

void LightDB::dumpInternal(WaitGroup& wg, std::string path, DataType eType){
    //printf("begin dump ! type:%d \n", eType);
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
    wg.Done();
}



Status LightDB::dumpStore(vector<DBFile*>& mergeFiles, std::string mergePath, Entry* e){
    //get the last file of the type
    Status s;
    DBFile* df = mergeFiles[mergeFiles.size()-1];
    if(df->Offset > config->blockSize){
        //free the last file
        delete(df);
        df = new DBFile(mergePath, df->Id+1, config->rWMethod, config->blockSize, e->GetType());
        mergeFiles.push_back(df);
    }
    s = df->Write(e);
    return s;
}




void LightDB::mergeString(WaitGroup& wg){

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

    auto archFiles = mergeFiles[String];
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


    for(auto it = archivedFiles[String].begin(); it!=archivedFiles[String].end(); it++){
        if(archFiles.find(it->first) == archFiles.end()){
        }
        //todo
        fileIds.push_back(it->first);
    }
    sort(fileIds.begin(), fileIds.end());

    for(int i = 0; i<fileIds.size(); i++){
        DBFile* dbFile = archivedFiles[String][i];
        vector<Entry*> validEntries;
        s = FindValidEntries(validEntries, dbFile);

        //rewrite the valid entries
        DBFile* rewriteDF = nullptr;
        int rewriteFileId = 0;
        for(auto entry : validEntries){
            if(rewriteDF == nullptr || rewriteDF->Offset > config->blockSize){
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
            if(this->strIdx.indexes->get(entry->meta->key, idx)){
                idx.fileId = rewriteFileId;
                idx.offset = rewriteDF->WriteOffset - entry->Size();
                strIdx.indexes->put(entry->meta->key, idx);
            }
        }
        //delete older dbFile
        dbFile->Close();
        std::remove((config->dirPath + "/" + dbFile->FileName()).c_str());
    }
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
    this->loadDBFiles(String);
    wg.Done();
    return;
}



Status LightDB::FindValidEntries(std::vector<Entry*>& entries, DBFile* df){
    Status s;
    uint64_t offset = 0;
    for(;;){
        printf("offset ------:%d \n", offset);
        Entry* e = new Entry();
        s = df->Read(offset, *e);
        if(!s.ok()){
            if(s.Code() == kEndOfFile){
                return Status::OK();
            }
            return s;
        }
        if(validEntry(e, offset, df->Id)){
            printf("valid entry !!!! \n");
            entries.push_back(e);
        }

        offset += e->Size();
    }
}

bool LightDB::validEntry(Entry* e, int64_t offset, uint32_t fileId){
    if(e == nullptr){
        return false;
    }

    if(expires[String].find(e->meta->key) != expires[String].end() &&
    expires[String][e->meta->key] > getCurrentTimeStamp()){
        return true;
    }

    if(e->GetMark() == StringSet){
        Indexer indexer;
        bool get = strIdx.indexes->get(e->meta->key, indexer);
        if(!get){
            return false;
        }
        if(indexer.meta->key.compare(e->meta->key) == 0){
            if(indexer.offset == offset && indexer.fileId == fileId){
                return true;
            }
        }
    }

    return false;
}


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
