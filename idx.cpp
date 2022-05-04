//
// Created by xiaolei on 2022/2/25.
//

#include "lightdb.h"
namespace lightdb{

/*
 * 遍历所有类型的db文件，建立索引，每次打开数据库的时候都要进行一遍恢复索引的操作
 */
Status LightDB::loadIdxFromFiles(){
    for(int dataType=0; dataType<DataStructureNum; dataType++){

        std::vector<uint32_t> fileIds;
        std::map<uint32_t, DBFile*> dbFiles;

        //archived files
        for(auto & it : archivedFiles[dataType]){
            dbFiles.insert(std::make_pair(it.first, it.second));
            fileIds.push_back(it.first);
        }

        //active files
        DBFile* activeFile = getActiveFile(static_cast<DataType>(dataType));
        dbFiles.insert(std::make_pair(activeFile->Id, activeFile));
        fileIds.push_back(activeFile->Id);

        sort(fileIds.begin(), fileIds.end());

        for(unsigned int fid : fileIds){
            DBFile* df = dbFiles[fid];
            int64_t offset = 0;
            for(; offset < config->blockSize; ){
                Entry e;
                Status s = df->Read(offset, e);
                if(s.Code() == kEndOfFile){
                    break;
                }
                if(!s.ok()){
                    printf("a fatal err occurred, the db can not open, code:%d \n", s.Code());
                    s.Report();
                    exit(1);
                }

                Indexer* indexer = new Indexer();
                indexer->fileId = fid;
                indexer->offset = offset;

                offset += e.Size();
                df->SetOffset(offset);
                if(!e.meta->key.empty()){
                    buildIndex(&e, indexer);
                }

            }
        }
    }
    return Status::OK();
}

// 只在打开数据库，遍历所有数据文件进行索引恢复时使用，往db文件追加entry时，也会对entry建立索引，但是不会用这个函数，会使用strset系列函数
Status LightDB::buildIndex(Entry* entry, Indexer* indexer){
    switch(entry->GetType()){
        case String:
            indexer->meta->key = entry->meta->key;
            if (config->indexMode == KeyValueMemMode) {
                indexer->meta->value = entry->meta->value;
            }
            buildStringIndex(indexer, entry);
            break;
        case List:
            buildListIndex(entry);
            break;
        case Hash:
            buildHashIndex(entry);
            break;
        case Set:
            buildSetIndex(entry);
            break;
        case ZSet:
            buildZSetIndex(entry);
            break;
        default:
            break;
    }
    return Status::OK();
}

//build
void LightDB::buildStringIndex(Indexer* indexer,Entry* entry){
    if(indexer== nullptr){
        return;
    }
    switch (entry->GetMark()) {
        case StringSet:
            indexer->meta->value = entry->meta->value;
            strIdx.indexes->put(indexer->meta->key, *indexer);
            break;
        case StringRem:
            strIdx.indexes->erase(indexer->meta->key);
            break;
        case StringExpire:
            if(entry->timeStamp<getCurrentTimeStamp()){
                strIdx.indexes->erase(indexer->meta->key);
            }else{
                expires[String][indexer->meta->key] = entry->timeStamp;
            }
        default:
            break;
    }
}

void LightDB::buildListIndex(Entry *entry) {
    if(entry == nullptr){
        return;
    }
    std::string key = entry->meta->key;
    std::string tmp;

    std::vector<std::string> pivot_and_opt;
    std::vector<std::string> start_and_end;
    switch (entry->GetMark()) {
        InsertOption opt;
        int count;
        case ListLPush:
            listIdx.indexes->LPush(key, entry->meta->value);
            break;

        case ListLPop:
            listIdx.indexes->LPop(key, tmp);
            break;

        case ListRPush:
            listIdx.indexes->RPush(key, entry->meta->value);
            break;

        case ListRPop:
            listIdx.indexes->RPop(key, tmp);
            break;

        case ListLRem:
            count = std::stoi(entry->meta->extra);
            listIdx.indexes->LRem(key, entry->meta->value, count);
            break;

        case ListLInsert:
            pivot_and_opt = splitWithStl(entry->meta->extra, ExtraSeparator);
            opt = static_cast<InsertOption>(std::stoi(pivot_and_opt[1]));
            listIdx.indexes->LInsert(entry->meta->key, opt, pivot_and_opt[0], entry->meta->value);
            break;

        case ListLSet:
            listIdx.indexes->LSet(entry->meta->key, std::stoi(entry->meta->extra), entry->meta->value);
            break;

        case ListLTrim:
            start_and_end = splitWithStl(entry->meta->extra, ExtraSeparator);
            listIdx.indexes->LTrim(entry->meta->key, std::stoi(start_and_end[0]), std::stoi(start_and_end[1]));
            break;

        case ListLExpire:
            if(entry->timeStamp < getCurrentTimeStamp()){
                listIdx.indexes->LClear(entry->meta->key);
            }else{
                expires[List][entry->meta->key] = entry->timeStamp;
            }
            break;

        case ListLClear:
            listIdx.indexes->LClear(entry->meta->key);
        default:
            break;
    }

}

//build hashIdx, given a hash entry.
void LightDB::buildHashIndex(Entry *entry) {
    if(entry == nullptr){
        return;
    }
    std::string key = entry->meta->key;
    switch (entry->GetMark()) {
        case HashHSet:
            hashIdx.indexes->HSet(entry->meta->key, entry->meta->extra, entry->meta->value);
            break;
        case HashHDel:
            hashIdx.indexes->HDel(entry->meta->key, entry->meta->extra);
            break;
        case HashHClear:
            hashIdx.indexes->HClear(entry->meta->key);
            break;
        case HashHExpire:
            if(entry->timeStamp < getCurrentTimeStamp()){
                hashIdx.indexes->HClear(entry->meta->key);
            }else{
                expires[Hash][entry->meta->key] = entry->timeStamp;
            }
        default:
            break;
    }
}

void LightDB::buildSetIndex(Entry *entry) {
    if(entry == nullptr){
        return;
    }
    std::string key = entry->meta->key;
    switch (entry->GetMark()) {
        case SetSAdd:
            setIdx.indexes->SAdd(key, entry->meta->value);
            break;
        case SetSRem:
            setIdx.indexes->SRem(key, entry->meta->value);
            break;
        case SetSMove:
            setIdx.indexes->SMove(key, entry->meta->extra, entry->meta->value);
            break;
        case SetSClear:
            setIdx.indexes->SClear(key);
            break;
        case SetSExpire:
            if(entry->timeStamp < getCurrentTimeStamp()){
                setIdx.indexes->SClear(key);
            }else{
                expires[Set][key] = entry->timeStamp;
            }
        default:
            break;
    }
}

void LightDB::buildZSetIndex(Entry *entry) {
    if(entry == nullptr){
        return;
    }
    std::string key = entry->meta->key;
    double score;
    switch (entry->GetMark()) {
        case ZSetZAdd:
            score = atof(entry->meta->extra.c_str());
            sortedSetIdx.indexes->ZAdd(key, score, entry->meta->value);
            break;
        case ZSetZRem:
            sortedSetIdx.indexes->ZRem(key, entry->meta->value);
            break;
        case ZSetZClear:
            sortedSetIdx.indexes->ZClear(key);
            break;
        case ZSetZExpire:
            if(entry->timeStamp < getCurrentTimeStamp()){
                sortedSetIdx.indexes->ZClear(key);
            }else{
                expires[ZSet][key] = entry->timeStamp;
            }
        default:
            break;
    }

}


}//namespace lightdb

