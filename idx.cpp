//
// Created by 9 on 2022/2/25.
//

#include "lightdb.h"
namespace lightdb{

/*
 * load
 *
 * */
Status LightDB::loadIdxFromFiles(){
    for(int dataType=0; dataType<DataStructureNum; dataType++){
        printf("********buildIndex********\n");
        std::vector<uint32_t> fileIds;
        std::map<int,DBFile*> dbFiles;

        //archived files
        for(auto it=archivedFiles[dataType].begin(); it!=archivedFiles[dataType].end(); it++){
            dbFiles.insert(std::make_pair(it->first, it->second));
            fileIds.push_back(it->first);
        }

        //active files
        DBFile* activeFile = getActiveFile(static_cast<DataType>(dataType));
        dbFiles.insert(std::make_pair(activeFile->Id, activeFile));

        sort(fileIds.begin(), fileIds.end());

        for(int i = 0; i < fileIds.size(); i++){
            uint32_t fid = fileIds[i];
            DBFile* df = dbFiles[fid];
            int64_t offset = 0;
            for(;offset < config->blockSize;){
                Entry e;
                Status s = df->Read(e);
                if(s.Code() == kEndOfFile){
                    break;
                }
                if(!s.ok()){
                    printf("a fatal err occurred, the db can not open \n");
                    s.Report();
                    exit(1);
                }

                Indexer* indexer = new Indexer();
                indexer->meta = e.meta;
                indexer->fileId = fid;
                indexer->offset = offset;
                printf("build index, set offset key:%s, offset:%d \n", e.meta->key.c_str(), indexer->offset);

                offset+=e.Size();
                df->SetOffset(offset);
                if(e.meta->key.size() > 0){
                    buildIndex(&e, indexer, true);
                }

            }
        }

    }
}

Status LightDB::buildIndex(Entry* entry, Indexer* indexer, bool isOpen){
    if(config->indexMode == KeyValueMemMode && entry->GetType() == String){
        indexer->meta->value = entry->meta->value;
        indexer->meta->valueSize = entry->meta->valueSize;
    }
    printf("Type:%d \n",entry->GetType());
    switch(entry->GetType()){
        case String:
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
    }
}

//build
void LightDB::buildStringIndex(Indexer* indexer,Entry* entry){
    if(indexer== nullptr){
        return;
    }
    switch (entry->GetMark()) {
        case StringSet:
            printf("string put, key:%s, value:%s, offset:%d \n", indexer->meta->key.c_str(), indexer->meta->value.c_str(), indexer->offset);
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
    }

}

//build hashIdx, given a hash entry.
void LightDB::buildHashIndex(Entry *entry) {
    if(entry == nullptr){
        return;
    }
    std::string key = entry->meta->key;
    printf("Mark:%d \n",entry->GetMark());
    switch (entry->GetMark()) {
        case HashHSet:
            hashIdx.indexes->HSet(entry->meta->key, entry->meta->extra, entry->meta->value);
            printf("hashBuildIndex, key:%s, value:%s \n", entry->meta->key.c_str(), entry->meta->value.c_str());
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
    }

}


}//namespace lightdb

