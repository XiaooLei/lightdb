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
#include "util/time.h"
#include "stdio.h"
#include "sync/waitGroup.h"
#include <thread>
#include <cstdio>


namespace lightdb {
class LightDB{
    public:
    LightDB();

    Status Open(Config* config);

    Status Merge();

    void dump(WaitGroup& wg);

    Status dumpStore(vector<DBFile*>& mergeFiles, std::string mergePath, Entry* e);

    void mergeString(WaitGroup& wg);

    Status loadDBFiles(DataType dataType);

    bool validEntry(Entry* e, int64_t offset, uint32_t fileId);

    Status FindValidEntries(std::vector<Entry*>& entries, DBFile* df);

    void dumpInternal(WaitGroup& wg, std::string path, DataType eType);

    Status dumpList(vector<DBFile*>& mergeFiles, std::string path);

    Status dumpHash(vector<DBFile*>& mergeFiles, std::string path);

    Status dumpSet(vector<DBFile*>& mergeFiles, std::string path);

    Status dumpZSet(vector<DBFile*>& mergeFiles, std::string path);

    Status CheckKeyValue(std::string key, std::string value);

    Status CheckKeyValue(std::string key, std::vector<std::string> values);

    bool CheckExpired(std::string key, DataType dataType);

    Status store(Entry* entry);

    DBFile* getActiveFile(DataType dataType);

    //String operations

    Status StrKeys(std::vector<std::string> keys);

    Status StrValues(std::vector<std::string> values);

    Status SetIndexer(Entry* entry);

    Status StrSet(std::string key, std::string value);

    Status StrSetNx(std::string key, std::string value, bool& suc);

    Status StrSetEx(std::string key, std::string value, uint64_t duration);

    bool StrExist(std::string key);

    Status Get(std::string key, std::string& value, bool& suc);

    Status GetSet(std::string key, std::string& oldValue, const std::string& newValue, bool& suc);

    Status MSet(std::vector<std::string> values);

    Status MGet(const std::vector<std::string>& keys, std::vector<std::string>& values, std::vector<bool>& sucs);

    Status Append(std::string key, std::string value, int& length);

    Status Remove(std::string key, bool& suc);

    Status Expire(std::string key, uint64_t duration, bool& suc);

    int64_t TTL(std::string key);

    Status getVal(std::string key, std::string& value, bool& suc);

    

    //hash operations;
    Status HSet(std::string key, std::string field, std::string value, int& res);

    Status HSetNx(std::string key, std::string field, std::string value, bool& res);

    bool HGet(std::string key, std::string field, std::string& value);

    bool HGetAll(std::string key, std::vector<std::string>& vals);

    Status HMSet(std::string key, std::vector<std::string> vals);

    Status HDel(std::string key, std::string field, int& res);

    Status HMGet(std::string key, std::vector<std::string> fields, std::vector<std::string>& vals, std::vector<bool>& sucs);

    bool HKeyExist(std::string key);

    bool HExist(std::string key, std::string field);

    int HLen(std::string key);

    bool HKeys(std::string key, std::vector<std::string>& keys);

    bool HVals(std::string key, std::vector<std::string>& vals);

    Status HClear(std::string key, int& res);

    Status HExpire(std::string key, uint64_t duration, bool& suc);

    int64_t HTTL(std::string key);

    //List operations
    Status LPush(std::string key, std::string value, int& length);

    Status RPush(std::string key, std::string value, int& length);

    Status LPop(std::string key, std::string& val, bool& suc);

    Status RPop(std::string key, std::string& val, bool& suc);

    bool LIndex(std::string key, uint32_t index, std::string& val);

    Status LRem(std::string key, std::string value, int count, int& removed);

    Status LInsert(std::string key, InsertOption option, std::string pivot, std::string val, int& count);

    Status LSet(std::string key, int idx, std::string value, bool& suc);

    Status LTrim(std::string key, int start, int end, bool& suc);

    Status LRange(std::string key, int start, int end, std::vector<std::string>& vals, bool& suc);

    bool LKeyExist(std::string key);

    int LLen(std::string key);

    Status LClear(std::string key, int& count);

    Status LExpire(std::string key, uint64_t duration, bool& suc);

    int64_t LTTL(std::string key);


    //Set operations
    Status SAdd(std::string key, std::string member, int& count);

    Status SPop(std::string key, std::string& val, bool& suc);

    bool SIsMember(std::string key, std::string member);

    Status SRem(std::string key, std::string member, bool& suc);

    Status SMove(std::string src, std::string dst, std::string member, bool& suc);

    int SCard(std::string key);

    bool SMembers(std::string key, std::vector<std::string>& members);

    void SUnion(std::vector<std::string> keys, std::vector<std::string>& vals);

    void SDiff(std::string key1, std::vector<std::string> succ_keys, std::vector<std::string>& vals);

    bool SKeyExist(std::string key);

    Status SClear(std::string key, int& count);

    Status SExpire(std::string key, uint64_t duration, bool& suc);

    int64_t STTL(std::string key);

    //SortedSet operations
    Status ZAdd(std::string key, double score,std::string member, bool& suc);

    bool ZScore(std::string key, std::string member, double& score);

    int ZCard(std::string key);

    int ZRank(std::string, std::string member);

    int ZRevRank(std::string, std::string member);

    Status ZIncrBy(std::string key, double increment, std::string member, double& res);

    void ZRange(std::string key, int start, int end, std::vector<std::string>& vals);

    void ZRevRange(std::string key, int start, int end, std::vector<std::string>& vals);

    Status ZRem(std::string key, std::string member, bool& suc);

    bool ZGetByRank(std::string key, int rank, std::string& member);

    bool ZRevGetByRank(std::string key, int rank, std::string& member);

    bool ZKeyExist(std::string key);

    Status ZClear(std::string key, int& count);

    Status ZExpire(std::string key, int64_t duration, bool& suc);

    int64_t ZTTL(std::string key);

    //idx
    Status loadIdxFromFiles();

    Status buildIndex(Entry* entry, Indexer* indexer);

    void buildStringIndex(Indexer* indexer,Entry* entry);

    void buildListIndex(Entry* entry);

    void buildHashIndex(Entry* entry);

    void buildSetIndex(Entry* entry);

    void buildZSetIndex(Entry* entry);

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
