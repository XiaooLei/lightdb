#pragma once
#include <cstdlib>
#include <cassert>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


namespace lightdb{

enum Code {kOk, kIOError, kNotFound, kEndOfFile, kKeyNotExist, kKeyExpiredErr, kEmptyKey, kKeyTooLarge, kValueTooLarge,
           kHFieldNotExist,                                            //Hash
           kLInvalidIndex, kLInvalidRange, kLPivotNotExist,            //List
           kSmemberNotExist, kWrongNumberOfArgs,                       //Set
           kZMemberNotExist,kZInvalidRank,                             //ZSet
           kIsMering
           };

//用来存储异常信息
class Status{
    private:
    char* state_;


    //定义：code:每一种错误对应一个code
    //msg,用来描述错误类型, msg2,用来描述错误相关的具体信息
    Status(Code code, const std::string& msg, const std::string& msg2){
        assert(code != kOk);
        const uint32_t len1 = static_cast<uint32_t>(msg.size());
        const uint32_t len2 = static_cast<uint32_t>(msg2.size());
        const uint32_t size = len1 + (len2 ? (2 + len2) : 0);
        char* result = (char *)malloc(size + 5);
        memcpy(result, &size, sizeof(size));
        result[4] = static_cast<char>(code);
        memcpy(result + 5, msg.data(), len1);
        if (len2) {
            result[5 + len1] = ':';
            result[6 + len1] = ' ';
            memcpy(result + 7 + len1, msg2.data(), len2);
        }
        state_ = result;
    }


    public:
    Status(): state_(nullptr){}
public:
    //copy constructor of Status
    Status(const Status& s){
        state_ = nullptr;
        if(s.state_!= nullptr) {
            state_ = (char*)malloc(sizeof(s.state_));
            memcpy(this->state_, s.state_, sizeof(s.state_));
        }
    }
    ~Status(){
        if(state_!= nullptr) {
            //todo 解决的内存泄漏问题
            //free(state_);
        }
    }


    Code Code(){
        if(state_ == nullptr){
            return kOk;
        }
        int32_t ret = static_cast<int32_t>(state_[4]);
        return static_cast<lightdb::Code>(ret);
    }

    void Report(){
        printf("msg:%s \n",state_);
    }
    

    static Status OK() { return Status(); }

    //msg2表示不存的key
    static Status KeyNotExist(const std::string& msg2 = ""){
        return Status(kKeyNotExist, "key not exist err", msg2);
    }

    //传入命令名称
    static Status WrongNumberOfArgsErr(const std::string& command){
        return Status(kWrongNumberOfArgs, "wrong number of args err", command);
    }

    //key 过期
    static Status KeyExpiredErr(const std::string& key){
        return Status(kKeyExpiredErr, "key expired err", key);
    }

    static Status EmptyKeyError(){
        return Status(kEmptyKey, "empty key err", "");
    }
    //传入大于限制长度的key
    static Status KeyTooLargeErr(const std::string& key){
        return Status(kKeyTooLarge, "key too large err", key);
    }

    //传入大于限制长度的value
    static Status ValueTooLargeErr(const std::string& value){
        return Status(kValueTooLarge, "value too large err", value);
    }

    //传入不存的field
    static Status HFieldNotExist( const std::string& field){
        return Status(kHFieldNotExist, "field not exist", field);
    }


    static Status LInvalidIndex(std::string key, int index){
        char buf[100];
        memset(buf, '\0', sizeof(buf));
        sprintf(buf, "LInvalidIndex, key:%s,index:%d", key.c_str(), index);
        std::string msg;
        msg.assign(buf);
        return Status(kLInvalidIndex, msg, "");
    }

    static Status LInvalidRange(std::string key, int start, int end){
        char buf[100];
        memset(buf, '\0', sizeof(buf));
        sprintf(buf, "LInvalidRange, key:%s,start:%d,end:%d", key.c_str(), start, end);
        std::string msg;
        msg.assign(buf);        
        return Status(kLInvalidRange, msg, "");
    }

    static Status LPivotNotExist(std::string key, std::string pivot){
        return Status(kLPivotNotExist, "pivot not exist err", pivot);
    }

    static Status SMemberNotExist(std::string key, std::string member){
        return Status(kSmemberNotExist, key, member);
    }

    static Status ZMemberNotExist(std::string key, std::string member){
        std::string msg2 = "key:" + key + "," + member;
        return Status(kZMemberNotExist, "zset member not exist err", member);
    }

    static Status NotFound(const std::string& msg, const std::string& msg2 = "") {
        return Status(kNotFound, msg, msg2);
    }

    static Status ZInvalidRank(const std::string& msg){
        return Status(kZInvalidRank,"","");
    }

    static Status IOError(const std::string& msg, const std::string& msg2 = "") {
        return Status(kIOError, msg, msg2);
    }

    static Status EndOfFile(const std::string& fileName){
        return Status(kEndOfFile, "the end of file", fileName);
    }

    static Status IsMergingErr(){
        return Status(kIsMering, "the db is merging files", "");
    }
    

    bool ok() const { return (state_ == nullptr); }


    

};



}// namespace lightdb
