
#include "lightdb.h"
namespace lightdb{

    // ZAdd adds the specified member with the specified score to the sorted set stored at key.
    Status LightDB::ZAdd(std::string key, double score,std::string member, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            suc = false;
            return Status::OK();
        }
        double old_score;
        bool exist = sortedSetIdx.indexes->ZScore(key, member, old_score);
        if(exist){
            suc = false;
            return Status::OK();
        }
        sortedSetIdx.indexes->ZAdd(key, score, member);
        std::string extra = to_string(score);
        Entry* e = Entry::NewEntryNow(key, member, extra, ZSet, ZSetZAdd);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        suc = true;
        return Status::OK();
    }

    // ZScore returns the score of member in the sorted set at key.
    bool LightDB::ZScore(std::string key, std::string member, double& score){
        Status s;
        s = CheckKeyValue(key, member);
        if(!s.ok()){
            return false;
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return false;
        }
        return sortedSetIdx.indexes->ZScore(key, member, score);
    }

    // ZCard returns the sorted set cardinality (number of elements) of the sorted set stored at key.
    int LightDB::ZCard(std::string key){
        Status s;
        if(!s.ok()){
            return 0;
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return 0;
        }
        return sortedSetIdx.indexes->ZCard(key);
    }

    // ZRank returns the rank of member in the sorted set stored at key, with the scores ordered from low to high.
    // The rank (or index) is 0-based, which means that the member with the lowest score has rank 0.
    int LightDB::ZRank(std::string key, std::string member){
        Status s;
        s = CheckKeyValue(key, member);
        if(!s.ok()){
            return -1;
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return -1;
        }
        return sortedSetIdx.indexes->ZRank(key, member);
    }

    int LightDB::ZRevRank(std::string key, std::string member) {
        Status s;
        s = CheckKeyValue(key, member);
        if(!s.ok()){
            return -1;
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return -1;
        }
        return sortedSetIdx.indexes->ZRevRank(key, member);
    }

    // ZIncrBy increments the score of member in the sorted set stored at key by increment.
    // If member does not exist in the sorted set, it is added with increment as its score (as if its previous score was 0.0).
    // If key does not exist, a new sorted set with the specified member as its sole member is created.
    Status LightDB::ZIncrBy(std::string key, double increment, std::string member, double& res){
        Status s;
        s = CheckKeyValue(key, member);
        if(!s.ok()){
            return s;
        }
        CheckExpired(key, ZSet);
        res = sortedSetIdx.indexes->ZIncrBy(key, increment, member);
        std::string extra = to_string(increment);
        Entry* e = Entry::NewEntryNow(key, member, extra, ZSet, ZSetZAdd);
        s = store(e);
        if(s.ok()){
            return s;
        }
        return Status::OK();
    }

    // ZRange returns the specified range of elements in the sorted set stored at key.
    void LightDB::ZRange(std::string key, int start, int end, std::vector<std::string>& vals){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return;
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return;
        }
        sortedSetIdx.indexes->ZRange(key, start, end,vals);
        return;
    }

    void LightDB::ZRevRange(std::string key, int start, int end, std::vector<std::string>& vals){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return;
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return;
        }
        sortedSetIdx.indexes->ZRevRange(key, start, end,vals);
        return;
    }



    // ZRem removes the specified members from the sorted set stored at key. Non existing members are ignored.
    // An error is returned when key exists and does not hold a sorted set.
    Status LightDB::ZRem(std::string key, std::string member, bool& suc){
        Status s;
        s = CheckKeyValue(key, member);
        if(!s.ok()){
            suc = false;
            return Status::OK();
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            suc = false;
            return Status::OK();
        }
        suc = sortedSetIdx.indexes->ZRem(key, member);
        Entry* e = Entry::NewEntryNoExtra(key, member, ZSet, ZSetZRem);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // ZGetByRank get the member at key by rank, the rank is ordered from lowest to highest.
    // The rank of lowest is 0 and so on.
    bool LightDB::ZGetByRank(std::string key, int rank, std::string& member){
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return false;
        }
        return sortedSetIdx.indexes->ZGetByRank(key, rank, member);
    }

    bool LightDB::ZRevGetByRank(std::string key, int rank, std::string& member){
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return false;
        }
        return sortedSetIdx.indexes->ZRevGetByRank(key, rank, member);
    }

    // ZKeyExists check if the key exists in zset.
    bool LightDB::ZKeyExist(std::string key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return false;
        }
        return sortedSetIdx.indexes->ZKeyExist(key);
    }

    // ZClear clear the specified key in zset.
    Status LightDB::ZClear(std::string key, int& count){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            count = 0;
            return Status::OK();
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            count = 0;
            return Status::OK();
        }
        count = sortedSetIdx.indexes->ZClear(key);
        return Status::OK();
    }

    // ZExpire set expired time for the key in zset.
    Status LightDB::ZExpire(std::string key, int64_t duration, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            suc = false;
            return Status::OK();
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            suc = false;
            return Status::OK();
        }
        if(!ZKeyExist(key)){
            suc = false;
            return Status::OK();
        }
        uint64_t deadline = getCurrentTimeStamp() + duration * 1000;
        Entry* e = Entry::NewEntryWithExpire(key, "", deadline, ZSet, ZSetZExpire);
        s = store(e);
        if(!s.ok()){
            suc = false;
            return s;
        }
        expires[ZSet][key] = deadline;
        suc = true;
        return Status::OK();
    }

    int64_t LightDB::ZTTL(std::string key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return -2;
        }
        bool expired = CheckExpired(key, ZSet);
        if(expired){
            return -2;
        }
        if(!ZKeyExist(key)){
            return -2;
        }
        return (expires[ZSet][key] - getCurrentTimeStamp()) / 1000;
    }

}