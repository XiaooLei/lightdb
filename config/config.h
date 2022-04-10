#pragma once
#include <sys/types.h>
#include <string>
#include "../include/common.h"
#include "configParser.h"

using namespace std;
namespace lightdb{

const string DefaultAddr = "127.0.0.1:5200";
const string DefaultDirPath = "/tmp/lightdb/test";
const uint32_t DefaultBlockSize = 500  * 1024;// * 1024;
const uint32_t DefaultMaxKeySize = uint32_t(1 * 1024 * 1024);
const uint32_t DefaultMaxValueSize = uint32_t(8 * 1024 * 1024);
const uint32_t DefaultMergeThreshold = 64;
const uint64_t DefaultMergeCheckInterval = 1000 * 5;//24 * 3600 * 1000;
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

    static Config BuildConfig(const std::string& configPath){
        CConfigParser* configParser = new CConfigParser();
        configParser->Parser(configPath.c_str());
        std::string addr = configParser->GetDefConfig("config", "Addr", DefaultAddr);
        std::string dirPath = configParser->GetDefConfig("config", "DirPath", DefaultDirPath);
        std::string fileRwMethodStr = configParser->GetConfig("config", "FileRWMethod");
        FileRWMethod fileRwMethod;
        if(fileRwMethodStr=="FileIO"){
            fileRwMethod = FileIO;
        }else{
            fileRwMethod = MMap;
        }
        std::string indexMode = configParser->GetConfig("config", "IndexMode");
        DataIndexMode dataIndexMode;
        if(indexMode == "KeyOnlyMemMode"){
            dataIndexMode = KeyOnlyMemMode;
        }else{
            dataIndexMode = KeyValueMemMode;
        }
        std::string blockSizeStr = configParser->GetConfig("config", "BlockSize");
        uint32_t blockSize;
        if(blockSizeStr==""){
            blockSize == DefaultBlockSize;
        }else{
            blockSize = stoi(blockSizeStr);
        }
        std::string maxKeySizeStr = configParser->GetConfig("config", "MaxKeySize");
        uint32_t maxKeySize;
        if(maxKeySizeStr == ""){
            maxKeySize = DefaultMaxKeySize;
        }else{
            maxKeySize = stoi(maxKeySizeStr);
        }

        std::string maxValueSizeStr = configParser->GetConfig("config", "MaxValueSize");
        uint32_t maxValueSize;
        if(maxValueSizeStr == ""){
            maxValueSize = DefaultMaxValueSize;
        }else{
            maxValueSize = stoi(maxValueSizeStr);
        }

        std::string syncStr = configParser->GetConfig("config", "Sync");
        bool sync;
        if(syncStr == ""){
            sync = false;
        }else{
            sync = true;
        }

        std::string mergeThresholdStr = configParser->GetConfig("config", "MergeThreshold");
        int mergeThreshold;
        if(mergeThresholdStr == ""){
            mergeThreshold = DefaultMergeThreshold;
        }else{
            mergeThreshold = stoi(mergeThresholdStr);
        }

        std::string mergeCheckIntervalStr = configParser->GetConfig("config", "MergeCheckInterval");
        uint64_t mergeCheckInterval;
        if(mergeThresholdStr == ""){
            mergeCheckInterval = DefaultMergeCheckInterval;
        }else{
            mergeCheckInterval = strtoul(mergeCheckIntervalStr.c_str(), nullptr, 10);
        }

        std::string cacheCapacityStr = configParser->GetConfig("config", "CacheCapacity");
        int cacheCapacity;
        if(cacheCapacityStr == ""){
            cacheCapacity = DefaultCacheCapacity;
        }else{
            cacheCapacity = stoi(cacheCapacityStr);
        }
        return Config(addr, dirPath, fileRwMethod, dataIndexMode,
                      blockSize, maxKeySize, maxValueSize,
                      sync, mergeThreshold, mergeCheckInterval, cacheCapacity);

    }

    public:
    string addr;
    string dirPath;
    FileRWMethod rWMethod;
    DataIndexMode indexMode;
    uint64_t blockSize;// db文件的最大容量
    uint32_t maxKeySize;
    uint32_t maxValueSize;
    bool sync;
    int mergeThreshold;
    uint64_t mergeCheckInterval;
    int cacheCapacity;
};


};
