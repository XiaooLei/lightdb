#pragma once
#include <unordered_map>
#include <map>
#include <vector>
#include "ds/hash/ds_hash.h"
#include "ds/list/ds_list.h"
#include "ds/set/ds_set.h"
#include "ds/zset/ds_zset.h"
#include "ds/str/ds_strlist.h"
#include "cache/lru.h"
#include "config/config.h"
#include "include/status.h"
#include "include/types.h"
#include "storage/db_file.h"
#include "storage/entry.h"
#include <ctime>
#include <cstdio>
#include "sync/waitGroup.h"
#include <thread>
#include <cstdio>
#include "util/time.h"


namespace lightdb {
class LightDB{
    public:
    LightDB();

    Status Open(Config* config, bool merge = false);

    Status Merge();

    void dump();

    Status dumpStore(vector<DBFile*>& mergeFiles, std::string mergePath, Entry* e);

    void mergeString();

    Status loadDBFiles(DataType dataType);

    bool validEntry(Entry* e, int64_t offset, uint32_t fileId);

    Status FindValidEntries(std::vector<Entry*>& entries, DBFile* df);

    void dumpInternal(const std::string& path, DataType eType);

    Status dumpList(vector<DBFile*>& mergeFiles, const std::string& path);

    Status dumpHash(vector<DBFile*>& mergeFiles, const std::string& path);

    Status dumpSet(vector<DBFile*>& mergeFiles, const std::string& path);

    Status dumpZSet(vector<DBFile*>& mergeFiles, const std::string& path);

    Status CheckKeyValue(const std::string& key, const std::string& value);

    Status CheckKeyValue(const std::string& key, const std::vector<std::string>& values);

    bool CheckExpired(const std::string& key, DataType dataType);

    Status store(Entry* entry);

    DBFile* getActiveFile(DataType dataType);

    //String operations

    Status StrKeys(std::vector<std::string> keys);

    Status StrValues(std::vector<std::string> values);

    Status SetIndexer(Entry* entry);

    Status StrSet(const std::string& key, const std::string& value);

    Status StrSetNx(const std::string& key, const std::string& value, bool& suc);

    Status StrSetEx(const std::string& key, const std::string& value, uint64_t duration);

    bool StrExist(const std::string& key);

    Status Get(const std::string& key, std::string& value, bool& suc);

    Status GetSet(const std::string& key, std::string& oldValue, const std::string& newValue, bool& suc);

    Status MSet(std::vector<std::string> values);

    Status MGet(const std::vector<std::string>& keys, std::vector<std::string>& values, std::vector<bool>& sucs);

    Status Append(const std::string& key, const std::string& value, int& length);

    Status Remove(const std::string& key, bool& suc);

    Status Expire(const std::string& key, uint64_t duration, bool& suc);

    int64_t TTL(const std::string& key);

    Status getVal(const std::string& key, std::string& value, bool& suc);

    

    //hash operations;
    Status HSet(const std::string& key, const std::string& field, const std::string& value, int& res);

    Status HSetNx(const std::string& key, const std::string& field, const std::string& value, bool& res);

    bool HGet(const std::string& key, const std::string& field, std::string& value);

    bool HGetAll(const std::string& key, std::vector<std::string>& vals);

    Status HMSet(const std::string& key, std::vector<std::string> vals);

    Status HDel(const std::string& key, const std::string& field, int& res);

    Status HMGet(const std::string& key, const std::vector<std::string>& fields, std::vector<std::string>& vals, std::vector<bool>& sucs);

    bool HKeyExist(const std::string& key);

    bool HExist(const std::string& key, const std::string& field);

    int HLen(const std::string& key);

    bool HKeys(const std::string& key, std::vector<std::string>& keys);

    bool HVals(const std::string& key, std::vector<std::string>& vals);

    Status HClear(const std::string& key, int& res);

    Status HExpire(const std::string& key, uint64_t duration, bool& suc);

    int64_t HTTL(const std::string& key);

    //List operations
    Status LPush(const std::string& key, const std::string& value, int& length);

    Status RPush(const std::string& key, const std::string& value, int& length);

    Status LPop(const std::string& key, std::string& val, bool& suc);

    Status RPop(const std::string& key, std::string& val, bool& suc);

    bool LIndex(const std::string& key, uint32_t index, std::string& val);

    Status LRem(const std::string& key, const std::string& value, int count, int& removed);

    Status LInsert(const std::string& key, InsertOption option, const std::string& pivot, const std::string& val, int& count);

    Status LSet(const std::string& key, int idx, const std::string& value, bool& suc);

    Status LTrim(const std::string& key, int start, int end, bool& suc);

    Status LRange(const std::string& key, int start, int end, std::vector<std::string>& vals, bool& suc);

    bool LKeyExist(const std::string& key);

    int LLen(const std::string& key);

    Status LClear(const std::string& key, int& count);

    Status LExpire(const std::string& key, uint64_t duration, bool& suc);

    int64_t LTTL(const std::string& key);


    //Set operations
    Status SAdd(const std::string& key, const std::string& member, int& count);

    Status SPop(const std::string& key, std::string& val, bool& suc);

    bool SIsMember(const std::string& key, const std::string& member);

    Status SRem(const std::string& key, const std::string& member, bool& suc);

    Status SMove(const std::string& src, const std::string& dst, const std::string& member, bool& suc);

    int SCard(const std::string& key);

    bool SMembers(const std::string& key, std::vector<std::string>& members);

    void SUnion(const std::vector<std::string>& keys, std::vector<std::string>& vals);

    void SDiff(const std::string& key1, const std::vector<std::string>& succ_keys, std::vector<std::string>& vals);

    bool SKeyExist(const std::string& key);

    Status SClear(const std::string& key, int& count);

    Status SExpire(const std::string& key, uint64_t duration, bool& suc);

    int64_t STTL(const std::string& key);

    //SortedSet operations
    Status ZAdd(const std::string& key, double score,const std::string& member, bool& suc);

    bool ZScore(const std::string& key, const std::string& member, double& score);

    int ZCard(const std::string& key);

    int ZRank(const std::string&, const std::string& member);

    int ZRevRank(const std::string&, const std::string& member);

    Status ZIncrBy(const std::string& key, double increment, const std::string& member, double& res);

    void ZRange(const std::string& key, int start, int end, std::vector<std::string>& vals);

    void ZRevRange(const std::string& key, int start, int end, std::vector<std::string>& vals);

    Status ZRem(const std::string& key, const std::string& member, bool& suc);

    bool ZGetByRank(const std::string& key, int rank, std::string& member);

    bool ZRevGetByRank(const std::string& key, int rank, std::string& member);

    bool ZKeyExist(const std::string& key);

    Status ZClear(const std::string& key, int& count);

    Status ZExpire(const std::string& key, int64_t duration, bool& suc);

    int64_t ZTTL(const std::string& key);

    //idx
    Status loadIdxFromFiles();

    Status buildIndex(Entry* entry, Indexer* indexer);

    void buildStringIndex(Indexer* indexer,Entry* entry);

    void buildListIndex(Entry* entry);

    void buildHashIndex(Entry* entry);

    void buildSetIndex(Entry* entry);

    void buildZSetIndex(Entry* entry);

    std::string getDirPath(){
        return this->config->dirPath;
    }

    private:
    // 每种数据类型都有相应的存储索引的结构
    HashIdx hashIdx;
    ListIdx listIdx;
    SetIdx  setIdx;
    StrIdx strIdx;
    SortedSetIdx sortedSetIdx;
    std::unordered_map<uint16_t,std::unordered_map<uint32_t,DBFile*>> archivedFiles;
    // uint16_t是value的型号，uint32_t是文件Id，注意不是fileid
    std::unordered_map<uint16_t,DBFile*> activeFiles;// activeFiles->first是数据类型的型号（0-4），activeFiles->second是文件实例
    Config* config;
    std::unordered_map<uint16_t,std::unordered_map<std::string,uint64_t>> expires;// uint16_t是value的类型号（0-4），std::string是key，uint64_t是过期时间
    LRUCache* cache;// 只用于value类型为string的数据，每次添加k-v对的
    bool isMerging;

    
};

}// namespace lightdb;
