
#include "lightdb.h"


namespace lightdb{
    // HSet sets field in the hash stored at key to value. If key does not exist, a new key holding a hash is created and return 1.
    // If field already exists in the hash, it is overwritten and return 0.
    Status LightDB::HSet(const std::string& key, const std::string& field, const std::string& value, int& res){
        Status s;
        s = CheckKeyValue(key, field);
        if(!s.ok()){
            res = 0;
            return s;
        }

        s = CheckKeyValue(key, value);
        if(!s.ok()){
            res = 0;
            return s;
        }

        std::string oldValue;
        HGet(key, field, oldValue);
        if(oldValue == value){
            res = 1;
            return Status::OK();
        }
        res = hashIdx.indexes->HSet(key, field, value);

        Entry* e = Entry::NewEntryNow(key, value, field, Hash, HashHSet);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // HSetNx Sets field in the hash stored at key to value, only if field does not yet exist.
    // If key does not exist, a new key holding a hash is created. If field already exists, this operation has no effect.
    // Return if the operation is successful.
    Status LightDB::HSetNx(const std::string& key, const std::string& field, const std::string& value, bool& res){
        Status s;

        s = CheckKeyValue(key, field);
        if(!s.ok()){
            return s;
        }
        s = CheckKeyValue(key, value);
        if(!s.ok()){
            return s;
        }
        CheckExpired(key, Hash);
        res = hashIdx.indexes->HSetNx(key, field, value);
        if(!res){
            return Status::OK();
        }
        Entry* e = Entry::NewEntryNow(key, value, field, Hash, HashHSet);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // HGet returns the value associated with field in the hash stored at key.
    // if key and field exist, return true, else return false.
    bool LightDB::HGet(const std::string& key, const std::string& field, std::string& value){
        Status s;
        bool res;
        s = CheckKeyValue(key, field);
        if(!s.ok()){
            return false;
        }
        bool expired = CheckExpired(key, Hash);
        if(expired){
            return false;
        }
        res = hashIdx.indexes->HGet(key, field, value);
        return res;
    } 

    // HGetAll returns all fields and values of the hash stored at key.
    // In the returned value, every field name is followed by its value, so the length of the reply is twice the size of the hash.
    bool LightDB::HGetAll(const std::string& key, std::vector<std::string>& vals){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;
        }
        bool expired = CheckExpired(key, Hash);
        if(expired){
            return false;
        }
        return hashIdx.indexes->HGetAll(key, vals);
    }

    // HMSet set multiple hash fields to multiple values
    Status LightDB::HMSet(const std::string& key, std::vector<std::string> vals){
        Status s;
        s = CheckKeyValue(key, vals);
        if(!s.ok()){
            return s;
        }
        for(int i = 0; i<vals.size(); i+=2){
            int res;
            s = HSet(key, vals[i], vals[i+1], res);
            if(!s.ok()){
                return s;
            }
        }
        return Status::OK();
    }

    // HDel removes the specified fields from the hash stored at key.
    // Specified fields that do not exist within this hash are ignored.
    // If key does not exist, it is treated as an empty hash and this command returns false.
    Status LightDB::HDel(const std::string& key, const std::string& field, int& res){
        Status s;
        s = CheckKeyValue(key, field);
        if(!s.ok()){
            res = 0;
            return Status::OK();
        }

        bool expired = CheckExpired(key, Hash);
        if(!expired){
            //if expired, delete nothing
            res = 0;
            return Status::OK();
        }

        bool deleted = hashIdx.indexes->HDel(key, field);
        if(!deleted){
            res = 0;
            return Status::OK();
        }
        res = 1;
        Entry* e = Entry::NewEntryNow(key, "", field, Hash, HashHDel);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // HMGet get the values of all the given hash fields
    Status LightDB::HMGet(const std::string& key, const std::vector<std::string>& fields, std::vector<std::string>& vals, std::vector<bool>& sucs){
        Status s;
        s = CheckKeyValue(key, fields);
        if(!s.ok()){
            return s;
        }

        for(auto & field : fields){
            std::string val;
            bool suc = HGet(key, field, val);
            if(suc){
                vals.push_back(val);
                sucs.push_back(true);
            }else{
                vals.emplace_back("");
                sucs.push_back(false);
            }
        }
        return Status::OK();
    }

    // HKeyExists returns if the key is existed in hash.
    bool LightDB::HKeyExist(const std::string& key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;
        }
        bool expired = CheckExpired(key, Hash);
        if(expired){
            return false;
        }
        return hashIdx.indexes->HKeyExist(key);
    }

    bool LightDB::HExist(const std::string& key, const std::string& field){
        Status s;
        s = CheckKeyValue(key, field);
        if(!s.ok()){
            return false;
        }
        bool expired = CheckExpired(key, Hash);
        if(expired){
            return false;
        }
        return hashIdx.indexes->HExist(key, field);
    }

    int LightDB::HLen(const std::string& key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return 0;
        }
        bool expired = CheckExpired(key, Hash);
        if(expired){
            return 0;
        }
        return hashIdx.indexes->HLen(key);
    }

    // HKeys returns all field names in the hash stored at key.
    // if keynot exist, return false, else return true
    bool LightDB::HKeys(const std::string& key, std::vector<std::string>& keys){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;
        }
        bool expired = CheckExpired(key, Hash);
        if(expired){
            return false;
        }
        if(!HKeyExist(key)){
            return false;
        }
        hashIdx.indexes->HKeys(key, keys);
        return true;
    }

    // HVals returns all values in the hash stored at key.
    // if key does not exist, return false, else return true
    bool LightDB::HVals(const std::string& key, std::vector<std::string>& vals){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;
        }
        if(!HKeyExist(key)){
            return false;
        }
        bool expired = CheckExpired(key, Hash);
        if(expired){
            return false;
        }
        hashIdx.indexes->HVals(key, vals);
        return true;
    }

    // HClear clear the key in hash.
    Status LightDB::HClear(const std::string& key, int& res){
        Status s;
        bool expired = CheckExpired(key, Hash);
        if(expired){
            res = 0;
            return Status::OK();
        }
        Entry* e = Entry::NewEntryNoExtra(key, "", Hash, HashHClear);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        res = hashIdx.indexes->HClear(key);
        return Status::OK();
    }

    // HExpire set expired time for a hash key.
    Status LightDB::HExpire(const std::string& key, uint64_t duration, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            suc = false;
            return Status::OK();
        }
        if(!HKeyExist(key)){
            suc = false;
            return Status::OK();
        }
        uint64_t deadline;
        deadline = getCurrentTimeStamp() + duration * 1000;
        Entry* e = Entry::NewEntryWithExpire(key, "", deadline, Hash, HashHExpire);
        s = store(e);
        if(!s.ok()){
            suc = false;
            return s;
        }
        expires[Hash][key] = deadline;
        suc = true;
        return Status::OK();
    }

    // HTTL return time to live for the key.
    int64_t LightDB::HTTL(const std::string& key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return -2;
        }
        bool expired = CheckExpired(key, Hash);
        if(expired){
            return -2;
        }
        if(expires[Hash].find(key) == expires[Hash].end()){
            return -2;
        }
        return (expires[Hash][key] - getCurrentTimeStamp()) / 1000;
    }

}// namespace lightdb