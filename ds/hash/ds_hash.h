
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include "../../include/status.h"

namespace lightdb{

class Hash;
class Hash{
    private:

    public:
    std::unordered_map<std::string,std::unordered_map<std::string,std::string>> record;

    Hash() = default;

    // HSet Sets field in the hash stored at key to value. If key does not exist, a new key holding a hash is created and return 1
    // If field already exists in the hash, it is overwritten and return 0
    // 
    int HSet(const std::string& key,const std::string& field, const std::string& value){
        if(record.find(key) == record.end()){
            std::unordered_map<std::string,std::string> new_bucket;
            record.insert(make_pair(key,new_bucket));
        }
        if(record[key].find(field) == record[key].end()){
            record[key].insert(std::make_pair(field, value));
            return 1;
        }else{
            record[key][field] = value;
            return 0;
        }
    }

    // HSetNx sets field in the hash stored at key to value, only if field does not yet exist.
    // If key does not exist, a new key holding a hash is created. If field already exists, this operation has no effect.
    // Return if the operation successful
    bool HSetNx(const std::string& key, const std::string& field, const std::string& value){
        if(!HKeyExist(key)){
            std::unordered_map<std::string,std::string> new_bucket;
            record.insert(make_pair(key,new_bucket));
            record[key].insert(make_pair(field, value));
            return true;
        }
        if(record[key].find(field) == record[key].end()){
            record[key].insert(make_pair(field, value));
            return true;
        }
        return false;
    }

    // HGet returns the value associated with field in the hash stored at key.
    // if key and field both exist, return true
    // if either key or field does not exist, return false;
    bool HGet(const std::string& key, const std::string& field, std::string& value){
        if(record.find(key) == record.end()){
            return false;
        }

        if(record[key].find(field) == record[key].end()){
            return false;
        }
        value = record[key][field];
        return true;
    }

    // HDel removes the specified fields from the hash stored at key. Specified fields that do not exist within this hash are ignored.
    // If key does not exist, it is treated as an empty hash and this command returns false.
    bool HDel(const std::string& key, const std::string& field){
        if(record.find(key) == record.end()){
            return false;
        }
        if(record[key].find(field) == record[key].end()){
            return false;
        }
        record[key].erase(field);
        return true;
    }

    // HKeyExists returns if key exists in hash.
    bool HKeyExist(const std::string& key){
        if(record.find(key)!=record.end()){
            return true;
        }
        return false;
    }

    // HExists returns if field is an existing field in the hash stored at key.
    bool HExist(const std::string& key, const std::string& field){
        if(!HKeyExist(key)){
            return false;
        }

        if(record[key].find(field) == record[key].end()){
            return false;
        }
        return true;
    }

    // HLen returns the number of fields contained in the hash stored at key.
    int HLen(const std::string& key){
        if(!HKeyExist(key)){
            return 0;
        }
        return record[key].size();
    }

    // HKeys returns all field names in the hash stored at key.
    void HKeys(const std::string& key, std::vector<std::string>& res){
        if(record.find(key) == record.end()){
            return;
        }
        for(auto & it : record[key]){
            res.push_back(it.first);
        }
    }

    // HVals returns all values in the hash stored at key.
    void HVals(const std::string& key, std::vector<std::string>& values){
        if(!HKeyExist(key)){
            return;
        }
        for(auto & it : record[key]){
            values.push_back(it.second);
        }
    }
    
    // HGetAll returns all fields and values of the hash stored at key.
    // In the returned value, every field name is followed by its value, so the length of the reply is twice the size of the hash.
    // if key exist, return true, else return false;
    bool HGetAll(const std::string& key, std::vector<std::string>& vals){
        if(!HKeyExist(key)){
            return false;
        }
        for(auto & it : record[key]){
            vals.push_back(it.first);
            vals.push_back(it.second);
        }
        return true;
    }

    // HClear clear the key in hash.
    int HClear(const std::string& key){
        if(!HKeyExist(key)){
            return 0;
        }
        int res = record[key].size();
        record.erase(key);
        return res;
    }



};
class Hash;
struct HashIdx{
    Hash* indexes;

    HashIdx(){
        indexes = new Hash();
    }
};

}//namespace lightdb