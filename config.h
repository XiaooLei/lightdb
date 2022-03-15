#pragma once
#include <sys/types.h>
#include <string>
#include "include/common.h"
using namespace std;
namespace lightdb{

const string DefaultAddr = "127.0.0.1:5200";
const string DefaultDirPath = "/tmp/lightdb";
const uint32_t DefaultBlockSize = 16 * 1024 * 1024;
const uint32_t DefaultMaxKeySize = uint32_t(1 * 1024 * 1024);
const uint32_t DefaultMaxValueSize = uint32_t(8 * 1024 * 1024);
const uint32_t DefaultMergeThreshold = 64;
const uint64_t DefaultMergeCheckInterval = 24 * 3600 * 1000;
const int DefaultCacheCapacity = 0;

class Config{

    //构造函数
    public:
    Config(){}

    Config(string addr, string dirPath, FileRWMethod rWMethod, DataIndexMode indexMode, 
    uint64_t blockSize, uint32_t maxKeySize, uint32_t maxValueSize, 
    bool sync, int mergeThreshold, uint64_t mergeCheckInterval,int cacheCapacity):
    addr(addr), dirPath(dirPath), rWMethod(rWMethod), indexMode(indexMode),
    blockSize(blockSize), maxKeySize(maxKeySize), maxValueSize(maxValueSize),
     sync(sync), mergeThreshold(mergeThreshold), mergeCheckInterval(mergeCheckInterval),cacheCapacity(cacheCapacity){}

    //默认配置
    static Config DefaultConfig(){
        return Config(DefaultAddr, DefaultDirPath, FileIO, KeyOnlyMemMode,
         DefaultBlockSize, DefaultMaxKeySize, DefaultMaxValueSize,
         false, DefaultMergeThreshold, DefaultMergeCheckInterval, DefaultCacheCapacity);
    }

    public:
    string addr;
    string dirPath;
    FileRWMethod rWMethod;
    DataIndexMode indexMode;
    uint64_t blockSize;
    uint32_t maxKeySize;
    uint32_t maxValueSize;
    bool sync;
    int mergeThreshold;
    uint64_t mergeCheckInterval;
    int cacheCapacity;
};


};