
#include <unordered_map>
#include <vector>
#include <mutex>
#include <set>
#include "../../include/status.h"

namespace lightdb{

class Set{
    public:
        std::unordered_map<std::string,std::unordered_map<std::string,int>> record;
    public:

    // SKeyExists returns if the key exists.
    bool SKeyExist(std::string key){
        if(record.find(key) == record.end()){
            return false;
        }
        return true;
    }

    // SAdd Add the specified members to the set stored at key.
    // Specified members that are already a member of this set are ignored.
    // If key does not exist, a new set is created before adding the specified members.
    // return the length of the set stored at the key after the SAdd operation
    int SAdd(std::string key, std::string member){
        if(!SKeyExist(key)){
            record.insert(std::make_pair(key,std::unordered_map<std::string,int>()));
        }
        record[key].insert(std::make_pair(member,1));
        return record[key].size();
    }

    // SPop Removes and returns one or more random members from the set value store at key.
    // if the set does not exist, return false, else return true
    bool SPop(std::string key,  std::string& val){
        if(!SKeyExist(key)){
            return false;
        }
        
        val = (record[key].begin()->first);
        record[key].erase(record[key].begin());
        if(record[key].size() == 0){
            record.erase(key);
        }
        return true;
    }

    // SCard Returns the set cardinality (number of elements) of the set stored at key.
    int SCard(std::string key){
        if(!SKeyExist(key)){
            return 0;
        }
        return record[key].size();
    }

    // SIsMember Returns if member is a member of the set stored at key.
    bool SIsMember(std::string key, std::string member){
        if(!SKeyExist(key)){
            return false;
        }
        if(record[key].find(member)==record[key].end()){
            return false;
        }
        return true;
    }

    // SRem Remove the specified members from the set stored at key.
    // Specified members that are not a member of this set are ignored.
    // If key does not exist, it is treated as an empty set and this command returns 0.
    bool SRem(std::string key, std::string member){
        if(!SKeyExist(key)){
            return false;
        }
        if(record[key].find(member) == record[key].end()){
            return false;
        }
        record[key].erase(member);
        return true;
    }

    // SMove Move member from the set at source to the set at destination.
    // If the source set does not exist or does not contain the specified element,no operation is performed and returns 0.
    bool SMove(std::string src, std::string dst, std::string member){
        if(!SKeyExist(src)){
            return false;
        }

        if(record[src].find(member) == record[src].end()){
            return false;
        }
        record[src].erase(member);
        if(!SKeyExist(dst)){
            record.insert(std::make_pair(dst,std::unordered_map<std::string,int>()));
        }
        record[dst].insert(std::make_pair(member,1));
        return true;
    }

    // SMembers Returns all the members of the set value stored at key.
    // if the key does not exist or the set stored at the key is empty, return false, else return true
    bool SMembers(std::string key, std::vector<std::string>& vals){
        if(!SKeyExist(key)){
            return false;
        }
        for(auto it = record[key].begin(); it!=record[key].end(); it++){
            vals.push_back(it->first);
        }
        return true;
    }

    // SUnion Returns the members of the set resulting from the union of all the given sets.
    void SUnion(std::vector<std::string> keys, std::vector<std::string>& vals){
        std::unordered_map<std::string,int> map;
        for(auto iti = keys.begin(); iti!=keys.end();iti++){
            if(!SKeyExist(*iti)){
                continue;
            }
            for(auto itj = record[*iti].begin(); itj!= record[*iti].end(); itj++){
                map[itj->first] = 1;
            }
        }
        for(auto it = map.begin(); it!=map.end(); it++){
            vals.push_back(it->first);
        }
    }

    void SDiff(std::string key1, std::vector<std::string> succ_keys, std::vector<std::string>& vals){
        std::set<std::string> to_ret;
        for(auto it = record[key1].begin(); it != record[key1].end(); it++){
            to_ret.insert(it->first);
        }
        std::vector<std::string> succ_vals;
        SUnion(succ_keys, succ_vals);
        for(auto it : succ_vals){
            if(to_ret.find(it)!=to_ret.end()){
                to_ret.erase(it);
            }
        }
        for(auto val : to_ret){
            vals.push_back(val);
        }
    }


    // SClear clear the specified key in set.
    int SClear(std::string key){
        if(!SKeyExist(key)){
            return 0;
        }
        int count = record[key].size();
        record.erase(key);
        return count;
    }


};

struct SetIdx{
    Set* indexes;
    std::mutex mutex;
    SetIdx(){
        indexes = new Set();
    }
};

}//namespace lightdb
