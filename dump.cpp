//
// Created by xiaolei on 2022/2/27.
//

#include "lightdb.h"
namespace lightdb{
    Status LightDB::dumpList(vector<DBFile*>& mergeFiles, const std::string& path) {
        Status s;
        for(auto & it : listIdx.indexes->record){
            std::string key = it.first;
            for(const auto& it2 : listIdx.indexes->record[key]){
                Entry* e = Entry::NewEntryNoExtra(key, it2, List, ListRPush);
                s = dumpStore(mergeFiles, path, e);
                if(!s.ok()){
                    return s;
                }
            }
        }
        return s;
    }

    Status LightDB::dumpHash(vector<DBFile*>& mergeFiles, const std::string& path) {
        Status s;
        for(auto & it : hashIdx.indexes->record){
            std::string key = it.first;
            for(auto it2 = hashIdx.indexes->record[key].begin(); it2!=hashIdx.indexes->record[key].end(); it2++){
                Entry* e = Entry::NewEntryNow(key, it2->second, it2->first, Hash, HashHSet);
                s = dumpStore(mergeFiles, path, e);
                if(!s.ok()){
                    return s;
                }
            }
        }
        return s;
    }

    Status LightDB::dumpSet(vector<DBFile*>& mergeFiles, const std::string& path) {
        Status s;
        for(auto & it : setIdx.indexes->record){
            std::string key = it.first;
            for(auto it2 = setIdx.indexes->record[key].begin(); it2!=setIdx.indexes->record[key].end(); it2++){
                Entry* e = Entry::NewEntryNoExtra(key, it2->first, Set, SetSAdd);
                s = dumpStore(mergeFiles, path, e);
                if(!s.ok()){
                    return s;
                }
            }
        }
        return s;
    }

    Status LightDB::dumpZSet(vector<DBFile*>& mergeFiles, const std::string& path) {
        Status s;
        for(auto & it : sortedSetIdx.indexes->record){
            std::string key = it.first;
            Skiplist* list = &sortedSetIdx.indexes->record[key];

            for(list->Begin(); !list->End(); list->Next()){

                std::string extra = to_string(list->CurScore());
                //ZSet Add
                Entry* e = Entry::NewEntryNow(key, list->CurMember(), extra, ZSet, ZSetZAdd);
                s = dumpStore(mergeFiles, path, e);
                if(!s.ok()){
                    return s;
                }
            }
        }
    }



}

