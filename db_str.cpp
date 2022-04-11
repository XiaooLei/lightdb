

#include "lightdb.h"
#include <string>

namespace lightdb{
    // Set set key to hold the std::string value. If key already holds a value, it is overwritten.
    // Any previous time to live associated with the key is discarded on successful Set operation.
    Status LightDB::StrSet(std::string key, std::string value){
        Status s;
        s = CheckKeyValue(key, value);
        if(!s.ok()){
            return s;
        }
        
        Entry* e = Entry::NewEntryNoExtra(key, value, String, StringSet);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        expires[String].erase(key);
        s = SetIndexer(e);
        if(!s.ok()){
            return s;
        }
        // KeyValueMemMode的k-v对不用放到cache中，因为value已经存放到indexer中了
        if(config->indexMode == KeyOnlyMemMode) {
            cache->put(key, value);
        }
        return Status::OK();
    }

    // SetNx is short for "Set if not exists", set key to hold std::string value if key does not exist.
    // In that case, it is equal to Set. When key already holds a value, no operation is performed.
    Status LightDB::StrSetNx(std::string key, std::string value, bool& suc){
        Status s;
        bool exist;
        exist = StrExist(key);
        if(exist){
            suc = false;
            return Status::OK();
        }
        s = StrSet(key, value);
        if(!s.ok()){
            return s;
        }
        suc = true;
        return Status::OK();
    }

    // SetEx set key to hold the std::string value and set key to timeout after a given number of seconds.
    Status LightDB::StrSetEx(std::string key, std::string value, uint64_t duration){
        Status s;
        uint64_t deadline = getCurrentTimeStamp() + duration;
        Entry* e = Entry::NewEntryWithExpire(key, value, duration, String, StringExpire);
        s = store(e);
        if(!s.ok()){
            return s;
        }        
        s = SetIndexer(e);
        if(!s.ok()){
            return s;
        }
        expires[String][key] = deadline;
        if(config->indexMode == KeyOnlyMemMode) {
            cache->put(key, value);
        }
        return Status::OK();
    }

    // Get get the value of key. If the key doesn't exist suc is set to be false
    Status LightDB::Get(std::string key, std::string& value, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            suc = false;
            return Status::OK();
        }
        bool expired = CheckExpired(key, String);
        if(expired){
            suc = false;
            return Status::OK();
        }

        s = getVal(key, value, suc);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // GetSet set key to value and returns the old value stored at key.
    // If the key not exist, return an err.
    Status LightDB::GetSet(std::string key, std::string& oldValue, const std::string& newValue, bool& suc){
        Status s;
        s = Get(key, oldValue, suc);
        if(!s.ok()){
            return s;
        }
        return StrSet(key, newValue);
    }

    // MSet set multiple keys to multiple values
    // 对于存在的k-v对，直接跳过。否则追加到数据文件中，并对其建立索引
    Status LightDB::MSet(std::vector<std::string> values){
        Status s;
        int argSize = values.size();
        if(argSize%2 != 0){
            Status::WrongNumberOfArgsErr("MSet");
        }
        std::vector<std::string> keys;
        std::vector<std::string> vals;
        bool skip;
        for(int index = 0; index < argSize; index+=2){
            std::string key = values[index];
            std::string value = values[index+1];
            s = CheckKeyValue(key, value);
            if(!s.ok()){
                return s;
            }
            if(config->indexMode == KeyValueMemMode){
                std::string tmp;
                bool suc;
                s = getVal(key, tmp, suc);
                if(!s.ok()){
                    return s;
                }

                if(suc && tmp.compare(value) == 0){
                    skip = true;
                }
            }
            if(!skip){
                keys.push_back(key);
                vals.push_back(value);
            }
        }
        if(keys.size() == 0 || vals.size() == 0){
            return Status::OK();
        }

        for(int i = 0; i < keys.size(); i++){
            Entry* e = Entry::NewEntryNoExtra(keys[i], vals[i], String, StringSet);
            s = store(e);
            if(!s.ok()){
                return s;
            }
            expires[String].erase(keys[i]);
            s = SetIndexer(e);
            if(!s.ok()){
                return s;
            }
            if(config->indexMode == KeyOnlyMemMode) {
                cache->put(keys[i], vals[i]);
            }
        }

        return Status::OK();
    }

    // MGet get the values of all the given keys
    Status LightDB::MGet(const std::vector<std::string>& keys, std::vector<std::string>& values, std::vector<bool>& sucs){
        Status s;
        for(int i = 0; i < keys.size(); i++){
            s = CheckKeyValue(keys[i], "");
            if(!s.ok()){
                return s;
            }
        }
        for(int i = 0; i<keys.size(); i++){
            std::string tmp;
            bool suc;
            s = Get(keys[i], tmp, suc);
            if(!s.ok()){
                return s;
            }
            sucs.push_back(suc);
            values.push_back(tmp);
        }
        return Status::OK();
    }

    // Append if key already exists and is a std::string, this command appends the value at the end of the std::string.
    // If key does not exist it is created and set as an empty std::string, so Append will be similar to Set in this special case.
    // The original type of the value must be std::string, otherwise you will get a wrong value after appending.
    Status LightDB::Append(std::string key, std::string value, int& length){
        Status s;
        s = CheckKeyValue(key, value);
        if(!s.ok()){
            return s;
        }

        std::string oldValue;
        bool exist;
        s = Get(key, oldValue, exist);
        if(!s.ok()){
            return s;
        }

        if(!exist){
            oldValue = "";
        }
        std::string newValue = oldValue + value;
        length = newValue.size();
        return StrSet(key, newValue);
    }

    // Remove remove the value stored at key.
    Status LightDB::Remove(std::string key, bool& suc){
        std::lock_guard<std::mutex> lockGuard(this->strIdx.mtx);
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            suc = false;
            return Status::OK();
        }
        Entry* e = Entry::NewEntryNoExtra(key, "", String, StringRem);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        suc = strIdx.indexes->erase(key);
        expires[String].erase(key);
        if(config->indexMode == KeyOnlyMemMode) {
            cache->remove(key);
        }
        return Status::OK();
    }

    // Expire set the expiration time of the key.
    Status LightDB::Expire(std::string key, uint64_t duration, bool& suc){
        Status s;
        std::string value;
        s = getVal(key, value, suc);
        if(!s.ok()){
            return s;
        }
        if(!suc){
            return Status::OK();
        }
        uint64_t deadline = getCurrentTimeStamp() + duration * 1000;
        Entry* e = Entry::NewEntryWithExpire(key, value, deadline, String, StringExpire);
        
        s = store(e);
        if(!s.ok()){
            return s;
        }

        s = SetIndexer(e);
        if(!s.ok()){
            return s;
        }

        expires[String][key] = deadline;

        if(config->indexMode == KeyOnlyMemMode) {
            cache->put(key, value);
        }
        return Status::OK();
    }

    // TTL Time to live.
    int64_t LightDB::TTL(std::string key){
        Status s;
        if(expires[String].find(key) == expires[String].end()){
            return -2;
        }
        uint64_t deadline = expires[String][key];
        bool expired = CheckExpired(key, String);
        if(expired){
            return -2;
        }
        return (deadline - getCurrentTimeStamp()) / 1000;
    }


    // 根据保存模式，如果是KeyValueMemMode，那么Indexer中就要保存entry->value以及entry->extra
    Status LightDB::SetIndexer(Entry* entry){
        std::lock_guard<std::mutex> lockGuard(this->strIdx.mtx);
        DBFile* activeFile = getActiveFile(String);
        Meta* tmp_meta = new Meta();
        tmp_meta->key = entry->meta->key;

        Indexer* idx = new Indexer(tmp_meta, activeFile->Id, static_cast<int64_t>(activeFile->WriteOffset - entry->Size()));
        delete tmp_meta;

        if( config->indexMode == KeyValueMemMode ){
            idx->meta->value = entry->meta->value;
        }

        strIdx.indexes->put(entry->meta->key, *idx);
        //上一行实际上拷贝了新的idx，这里要delete旧的idx
        delete idx;
        return Status::OK();
    }

    bool LightDB::StrExist(std::string key){
        std::lock_guard<std::mutex> lockGuard(this->strIdx.mtx);
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;
        }
        Indexer indexer;
        bool exist = strIdx.indexes->get(key, indexer);
        return exist;
    }



    Status LightDB::getVal(std::string key, std::string& value, bool& suc){
        std::lock_guard<std::mutex> lockGuard(this->strIdx.mtx);
        Status s;
        Indexer idx;
        bool get = strIdx.indexes->get(key, idx);
        if(!get){
            suc = false;
            return Status::OK();
        }

        bool expired = CheckExpired(key, String);
        if(expired){
            return Status::OK();
        }

        if(config->indexMode == KeyValueMemMode){
            value = idx.meta->value;
            suc = true;
            return Status::OK();
        }

        if(config->indexMode == KeyOnlyMemMode){
            suc = true;
            // cache只用于value为string类型的数据
            bool cached = cache->get(key, value);
            if(cached){
                return Status::OK();
            }

            DBFile* df = this->getActiveFile(String);
            if(idx.fileId != df->Id){
                df = archivedFiles[String][idx.fileId];
            }
            Entry entry;
            s = df->Read(idx.offset, entry);
            if(!s.ok()){
                return s;
            }
            value = entry.meta->value;
            // 从lrucache中读到entry后，要将这个节点移动到链表头部
            cache->put(key, value);
            return Status::OK();
        }
        return Status::OK();
    }

}//namespace lightdb
