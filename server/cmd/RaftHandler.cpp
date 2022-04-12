//
// Created by xiaolei on 2022/4/12.
//

#include "RaftHandler.h"

enum CommandType{Read, Write, RaftCommand};

std::map<CommandType, std::set<std::string>> CommandTypeMap = {
        {Read, {"get", "mget", "strexist", "ttl", "hget", "hgetall", "hmget", "hkeyexist", "hexist", "hlen",
                "hkeys", "hvals", "httl", "lindex", "lrange", "llen", "lkeyexist", "lttl", "sismember", "scard",
                "smembers", "sunion", "sdiff", "skeyexist", "sttl", "zscore", "zcard", "zrank", "zrevrank", "zrange",
                "zrevrange", "zgetbyrank", "zrevgetbyrank", "zkeyexist", "zttl"}},
        {Write,{"set", "setnx", "getset", "mset", "appendstr", "remove", "expire",
                "lpush", "rpush", "lpop", "rpop", "lrem", "linsert", "lset", "ltrim", "lclear", "lexpire",
                "sadd", "spop", "srem", "smove", "sclear", "sexpire",
                "hset", "hsetnx", "hmset", "hdel", "hclear", "hexpire",
                "zadd", "zincrby", "zrem", "zclear", "zexpire"}},
        {RaftCommand,{"appendentries, requestvote"}}};

namespace lightdb{


    std::string RaftHandler::HandleCmd(std::string request, int conn_fd) {
        //命令分发
        std::vector<std::string> cmdAndArgs;
        splitStrBySpace(request, cmdAndArgs);
        if(cmdAndArgs.size() == 0){
            return "";
        }
        std::string cmd = cmdAndArgs[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        if(CommandTypeMap[Read].find(cmd) != CommandTypeMap[Read].end()){
            if(getRaftState() == Leader) {
                //redirect;
                std::string redirectAddr;
                int redirectPort;
                GetRedirectServer(Follower, redirectAddr, redirectPort);
                return WrapDirectResp(redirectAddr, redirectPort);
            }



            //todo 处理读请求
        }else if(CommandTypeMap[Write].find(cmd) != CommandTypeMap[Write].end()){
            if(getRaftState() == Follower){
                //redirect
                std::string redirectAddr;
                int redirectPort;
                GetRedirectServer(Leader, redirectAddr, redirectPort);
                return WrapDirectResp(redirectAddr, redirectPort);
            }

            //todo 处理写请求
        }else if(CommandTypeMap[RaftCommand].find(cmd) != CommandTypeMap[RaftCommand].end()){

            //todo 处理raft命令
        }

        //todo
        return "";
    }

    RaftState RaftHandler::getRaftState() {
        return this->raft->getState();
    }

    void RaftHandler::GetRedirectServer(RaftState redirectTo, std::string& redirectToAddress, int& redirectToPort, int& redirectToServerNum){
        if(redirectTo == Leader){
            this->raft->getLeaderAddressAndPort(redirectToAddress, redirectToPort);
        }else if(redirectTo == Follower){
            this->raft->getFollowerAddressAndPort(redirectToAddress, redirectToPort);
        }else{
            //skip
        }
    }

    std::string RaftHandler::WrapDirectResp(std::string redirectToAddress, int redirectToPort, int redirectToServerNum) {
        //todo
        RaftServer raftServer(redirectToAddress, redirectToPort, redirectToServerNum);

    }

}
