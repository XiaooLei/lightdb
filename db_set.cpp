
#include "lightdb.h"
namespace lightdb{
    //static const int Set = 3;
    // SAdd add the specified members to the set stored at key.
    // Specified members that are already a member of this set are ignored.
    // If key does not exist, a new set is created before adding the specified members.
    Status LightDB::SAdd(std::string key, std::string member, int& count){
        Status s;
        s = CheckKeyValue(key, member);
        if(!s.ok()){
            return s;
        }
        bool expired = CheckExpired(key, Set);

        bool isMember = setIdx.indexes->SIsMember(key, member);

        count = setIdx.indexes->SAdd(key, member);

        if(isMember){
            return Status::OK();
        }
        Entry* e = Entry::NewEntryNoExtra(key, member, Set, SetSAdd);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // SPop removes and returns one or more random members from the set value store at key.
    Status LightDB::SPop(std::string key, std::string& val, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return s;
        }
        bool expired = CheckExpired(key, Set);
        if(expired){
            suc = false;
            return Status::OK();
        }
        suc = setIdx.indexes->SPop(key, val);
        Entry* e = Entry::NewEntryNoExtra(key, val, Set, SetSRem);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // SIsMember Returns if member is a member of the set stored at key.
    bool LightDB::SIsMember(std::string key, std::string member){
        Status s;
        s = CheckKeyValue(key, member);
        if(!s.ok()){
            return false;
        }
        bool expired = CheckExpired(key, Set);
        if(expired){
            return false;
        }
        bool exist = setIdx.indexes->SIsMember(key, member);
        return exist;
    }

    // SRem Remove the specified members from the set stored at key.
    // Specified members that are not a member of this set are ignored.
    // If key does not exist, it is treated as an empty set and this command returns 0.
    Status LightDB::SRem(std::string key, std::string member, bool& suc){
        Status s;
        s = CheckKeyValue(key, member);
        if(!s.ok()){
            return s;
        }
        bool expired = CheckExpired(key, Set);
        if(expired){
            suc = false;
            return Status::OK();
        }
        suc = setIdx.indexes->SRem(key, member);
        Entry* e = Entry::NewEntryNoExtra(key, member, Set, SetSRem);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // SMove move member from the set at source to the set at destination.
    Status LightDB::SMove(std::string src, std::string dst, std::string member, bool& suc){
        Status s;
        s = CheckKeyValue(src, "");
        if(!s.ok()){
            suc = false;
            return Status::OK();
        }
        s = CheckKeyValue(dst, member);
        if(!s.ok()){
            suc = false;
            return Status::OK();
        }
        bool srcExpired = CheckExpired(src, Set);
        if(srcExpired){
            suc = false;
            return Status::OK();
        }

        bool dstExpired = CheckExpired(dst, Set);
        suc = setIdx.indexes->SMove(src, dst, member);

        Entry* e = Entry::NewEntryNow(src, member, dst, Set, SetSMove);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // SCard returns the set cardinality (number of elements) of the set stored at key.
    int LightDB::SCard(std::string key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return 0;
        }
        bool expired = CheckExpired(key, Set);
        if(expired){
            return 0;
        }
        return setIdx.indexes->SCard(key);    
    }

    // SMembers returns all the members of the set value stored at key.
    bool LightDB::SMembers(std::string key, std::vector<std::string>& members){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;
        }
        bool expired = CheckExpired(key, Set);
        if(expired){
            return false;
        }
        return setIdx.indexes->SMembers(key, members);
    }

    // SUnion returns the members of the set resulting from the union of all the given sets.
    void LightDB::SUnion(std::vector<std::string> keys, std::vector<std::string>& vals){
        Status s;
        std::vector<std::string> validKeys;
        for(auto it = keys.begin(); it!=keys.end(); it++){
            s = CheckKeyValue(*it, "");
            if(s.ok()){
                validKeys.push_back(*it);
            }
        }
        std::vector<std::string> keysNotExpired;
        for(auto it = validKeys.begin(); it!=validKeys.end(); it++){
            if(!CheckExpired(*it, Set)){
                keysNotExpired.push_back(*it);
            }
        }
        setIdx.indexes->SUnion(keysNotExpired, vals);
    }

    void LightDB::SDiff(std::string key1, std::vector<std::string> succ_keys, std::vector<std::string>& vals) {
        Status s;
        s = CheckKeyValue(key1, "");
        if(!s.ok()){
            return;
        }
        if(CheckExpired(key1, Set)){
            return;
        }

        std::vector<std::string> validKeys;
        for(auto it = succ_keys.begin(); it!=succ_keys.end(); it++){
            s = CheckKeyValue(*it, "");
            if(s.ok()){
                validKeys.push_back(*it);
            }
        }
        std::vector<std::string> keysNotExpired;
        for(auto it = validKeys.begin(); it!=validKeys.end(); it++){
            if(!CheckExpired(*it, Set)){
                keysNotExpired.push_back(*it);
            }
        }
        setIdx.indexes->SDiff(key1, keysNotExpired, vals);
    }

    // SKeyExists returns if the key exists.
    bool LightDB::SKeyExist(std::string key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;        
        }
        bool expired = CheckExpired(key, Set);
        if(expired){
            return false;
        }
        return setIdx.indexes->SKeyExist(key);
    }

    // SClear clear the specified key in set.
    Status LightDB::SClear(std::string key, int& count){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            count = 0;
            return Status::OK();
        }
        if(!SKeyExist(key)){
            count = 0;
            return Status::OK();
        }
        bool expired = CheckExpired(key, Set);
        if(expired){
            count = 0;
            return Status::OK();
        }
        Entry* e = Entry::NewEntryNoExtra(key, "", Set, SetSClear);
        s = store(e);
        if(!s.ok()){
            return s;
        }        
        count = setIdx.indexes->SClear(key);
        return Status::OK();
    }

    // SExpire set expired time for the key in set.
    // if the key does not exist, suc = false, else suc = true
    Status LightDB::SExpire(std::string key, uint64_t duration, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            suc = false;
            return Status::OK();
        }
        bool expired = CheckExpired(key, Set);
        if(expired){
            suc = false;
            return Status::OK();
        }
        if(!SKeyExist(key)){
            suc = false;
            return Status::OK();
        }
        
        uint64_t deadline = getCurrentTimeStamp() + duration * 1000;
        Entry* e = Entry::NewEntryWithExpire(key, "", deadline, Set, SetSExpire);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        expires[Set][key] == deadline;
        suc = true;
        return Status::OK();
    }

    // STTL return time to live for the key in set.
    int64_t LightDB::STTL(std::string key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return -2;
        }
        bool expired = CheckExpired(key, Set);
        if(expired){
            return -2;
        }
        if(expires[Set].find(key) == expires[static_cast<const uint16_t>(Set)].end()){
            return -2;
        }
        return (expires[Set][key] - getCurrentTimeStamp()) / 1000;
    }
    

}//namespace lightdb