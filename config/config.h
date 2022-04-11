#pragma once
#include <sys/types.h>
#include <string>
#include "../include/common.h"
#include "configParser.h"
#include "../util/dir_util.h"

using namespace std;
namespace lightdb{

const string DefaultAddr = "127.0.0.1:5200";
const string DefaultDirPath = "/tmp/test/lightdb";
const uint32_t DefaultBlockSize = 500  * 1024;// * 1024;
const uint32_t DefaultMaxKeySize = uint32_t(1 * 1024 * 1024);
const uint32_t DefaultMaxValueSize = uint32_t(8 * 1024 * 1024);
const uint32_t DefaultMergeThreshold = 64;
const uint64_t DefaultMergeCheckInterval = 1000 * 24; //* 3600 * 1000;
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
     sync(sync), mergeThreshold(mergeThreshold), mergeCheckInterval(mergeCheckInterval),cacheCapacity(cacheCapacity){
        dataPath = dirPath + "/data";
    }

    //默认配置
    static Config DefaultConfig(){
        return Config(DefaultAddr, DefaultDirPath, FileIO, KeyOnlyMemMode,
         DefaultBlockSize, DefaultMaxKeySize, DefaultMaxValueSize,
         false, DefaultMergeThreshold, DefaultMergeCheckInterval, DefaultCacheCapacity);
    }

    static Config BuildConfig(const std::string& configPath){
        ConfigParser* configParser = new ConfigParser();
        configParser->Parser(configPath.c_str());
        std::string addr = configParser->GetDefConfig("Addr", DefaultAddr);
        std::string dirPath = configParser->GetDefConfig( "DirPath", DefaultDirPath);
        std::string fileRwMethodStr = configParser->GetDefConfig( "FileRWMethod", "FileIO");
        FileRWMethod fileRwMethod;
        if(fileRwMethodStr=="FileIO"){
            fileRwMethod = FileIO;
        }else{
            fileRwMethod = MMap;
        }
        std::string indexMode = configParser->GetDefConfig("IndexMode", "KeyOnlyMemMode");
        DataIndexMode dataIndexMode;
        if(indexMode == "KeyOnlyMemMode") {
            dataIndexMode = KeyValueMemMode;
        }else{
            dataIndexMode = KeyValueMemMode;
        }
        std::string blockSizeStr = configParser->GetDefConfig("BlockSize", to_string(DefaultBlockSize));
        uint32_t blockSize;

        blockSize = stoi(blockSizeStr);

        std::string maxKeySizeStr = configParser->GetDefConfig( "MaxKeySize", to_string(DefaultMaxKeySize));
        uint32_t maxKeySize;
        maxKeySize = stoi(maxKeySizeStr);

        std::string maxValueSizeStr = configParser->GetDefConfig("MaxValueSize", to_string(DefaultMaxValueSize));
        uint32_t maxValueSize;
        maxValueSize = stoi(maxValueSizeStr);

        std::string syncStr = configParser->GetDefConfig("Sync", "0");
        bool sync;
        if(syncStr == "False"){
            sync = false;
        }else{
            sync = true;
        }

        std::string mergeThresholdStr = configParser->GetDefConfig("MergeThreshold", to_string(DefaultMergeThreshold));
        int mergeThreshold;
        mergeThreshold = stoi(mergeThresholdStr);

        std::string mergeCheckIntervalStr = configParser->GetDefConfig("MergeCheckInterval", to_string(DefaultMergeCheckInterval));
        uint64_t mergeCheckInterval;
        mergeCheckInterval = strtoul(mergeCheckIntervalStr.c_str(), nullptr, 10);

        std::string cacheCapacityStr = configParser->GetDefConfig("CacheCapacity", to_string(DefaultCacheCapacity));
        int cacheCapacity;
        cacheCapacity = stoi(cacheCapacityStr);
        return Config(addr, dirPath, fileRwMethod, dataIndexMode,
                      blockSize, maxKeySize, maxValueSize,
                      sync, mergeThreshold, mergeCheckInterval, cacheCapacity);
    }


    void InitDirs(){
        //init dirPath
        CreateDir(this->dirPath.c_str());
        CreateDir((this->dirPath + "/" + "data/").c_str());
    }

    public:
    string addr;
    string dirPath;
    string dataPath;
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
