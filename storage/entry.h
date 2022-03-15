#ifndef __SOMEFILE_H__
#define __SOMEFILE_H__

#include <sys/types.h>
#include <chrono>
#include <string>
#include "../include/common.h"
#include "../util/coding.h"
#include "../util/crc.h"
#include "../include/types.h"

namespace lightdb{

struct  Meta{
    std::string key;
    std::string value;
    std::string extra;
    uint32_t keySize;
    uint32_t valueSize;
    uint32_t extraSize;

    Meta(std::string key, std::string value, std::string extra, uint32_t keySize, uint32_t valueSize, uint32_t extraSize):
    key(key), value(value), extra(extra), keySize(keySize), valueSize(valueSize), extraSize(extraSize)
    {
    }
    Meta(){};

};

class Entry{
    public:
    Meta* meta;

    uint16_t state;

    uint32_t crc32;

    uint64_t txId;

    uint64_t timeStamp;

    public:
    Entry(std::string key, std::string value, std::string extra, uint16_t state, uint64_t timeStamp):state(state), timeStamp(timeStamp){
        meta = new Meta(key, value, extra, key.size(), value.size(), extra.size());
    }

    Entry(){
        meta = new Meta();
    }
            
    ~Entry(){
        delete meta;
    }

    uint32_t Size(){
        return EntryHeaderSize + meta->keySize + meta->valueSize + meta->extraSize;
    }

    //小端序编码
    int Encode(std::string& buf){
        uint32_t crc = CRC_Compute(meta->value);
        PutFixed32(&buf, crc);
        PutFixed32(&buf, meta->keySize);
        PutFixed32(&buf, meta->valueSize);
        PutFixed32(&buf, meta->extraSize);
        PutFixed16(&buf, state);
        PutFixed64(&buf, timeStamp);
        PutFixed64(&buf, txId);
        buf.append(meta->key);
        buf.append(meta->value);
        if(meta->extraSize > 0){
            buf.append(meta->extra);
        }
        return 0;
    }

    int Decode(std::string& buf){
        meta->keySize = DecodeFixed32(buf.c_str() + 4);
        //printf("decoded keysize :%d \n", meta->keySize);
        meta->valueSize = DecodeFixed32(buf.c_str() + 8);
        meta->extraSize = DecodeFixed32(buf.c_str() + 12);
        state = DecodeFixed16(buf.c_str() + 16);
        timeStamp = DecodeFixed64(buf.c_str() + 18);
        txId = DecodeFixed64(buf.c_str() + 22);
        //printf("after decode, keysize:%d valuesize %d extrasize %d \n", meta->keySize, meta->valueSize, meta->extraSize);
        return 0;
    }

    uint16_t GetType(){
        return state>>8;
    }

    uint16_t GetMark(){
        return state & (127);
    }

    static Entry* NewEntryNow(std::string key, std::string value, std::string extra, DataType type, OperationType mark){
        //set type and mark, higher 8 bits as data type, lower 8 bits as operation mark;
        uint16_t state = 0;
        state = state | (type<<8);
        state = state | mark;
        auto timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        return new Entry(key, value, extra, state, timeNow.count());
    }

    // NewEntryNoExtra create a new entry without extra info.
    static Entry* NewEntryNoExtra(std::string key, std::string value, DataType type, OperationType mark){
        uint16_t state;
        state = state | (type<<8);
        state = state | mark;
        auto timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        return new Entry(key, value, "", state, timeNow.count());
    }

    // NewEntryWithExpire create a new Entry with expired info.
    static Entry* NewEntryWithExpire(std::string key, std::string value, uint64_t deadline, DataType type, OperationType mark){
        uint16_t state;
        state = state | (type<<8);
        state = state | mark;
        return new Entry(key, value, "", state, deadline);
    }

};

}//namespace lightdb;

#endif