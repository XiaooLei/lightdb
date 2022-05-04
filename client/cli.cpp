//
// Created by xiaolei on 2022/3/23.
//

#include "cluster_client.h"
#include <vector>
#include <iostream>
using namespace lightdb;

std::map<RequestType, std::set<std::string>> CommandTypeMap = {
        {ReadReq, {"get", "mget", "strexist", "ttl", "hget", "hgetall", "hmget", "hkeyexist", "hexist", "hlen",
                "hkeys", "hvals", "httl", "lindex", "lrange", "llen", "lkeyexist", "lttl", "sismember", "scard",
                "smembers", "sunion", "sdiff", "skeyexist", "sttl", "zscore", "zcard", "zrank", "zrevrank", "zrange",
                "zrevrange", "zgetbyrank", "zrevgetbyrank", "zkeyexist", "zttl"}},
        {WriteReq,{"set", "setnx", "getset", "mset", "appendstr", "remove", "expire",
                "lpush", "rpush", "lpop", "rpop", "lrem", "linsert", "lset", "ltrim", "lclear", "lexpire",
                "sadd", "spop", "srem", "smove", "sclear", "sexpire",
                "hset", "hsetnx", "hmset", "hdel", "hclear", "hexpire",
                "zadd", "zincrby", "zrem", "zclear", "zexpire"}},
        {VoteReq,{"requestvote"}},
        {AppendEntriesReq, {"appendentries"}}};

int main(int argc, char* argv[]){
    ClusterClient clusterClient;
    if( argc != 3){
        printf("wrong args count \n");
        exit(0);
    }
    char* address = argv[1];
    int port = std::strtol(argv[2], nullptr, 10);

    if(clusterClient.Connect(address, port) < 0){
        printf("connect failed");
        exit(0);
    }
    printf("successfully connected to lighted,read remote:[%s:%d], write remote:[%s:%d]\n",
           clusterClient.ReadRemoteAddress().c_str(), clusterClient.ReadRemotePort(),
           clusterClient.WriteRemoteAddress().c_str(), clusterClient.WriteRemotePort());
    while(true){
        printf("(read:[%s:%d],write:[%s:%d])>", clusterClient.ReadRemoteAddress().c_str(), clusterClient.ReadRemotePort(),
               clusterClient.WriteRemoteAddress().c_str(), clusterClient.WriteRemotePort());
        std::string requestStr;
        std::getline(std::cin, requestStr);
        if(requestStr.empty()){
            continue;
        }
        if(requestStr == "quit"){
            clusterClient.Close();
            exit(0);
        }

        Response resp;
        std::vector<std::string> cmdAndArgs;
        splitStrBySpace(requestStr, cmdAndArgs);
        if(cmdAndArgs.empty()){
            return -1;
        }
        std::string cmd = cmdAndArgs[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if(CommandTypeMap[ReadReq].find(cmd) != CommandTypeMap[ReadReq].end()){
            Request request(ReadReq, requestStr);
            if(clusterClient.Execute(request, resp) < 0){
                printf("err occur\n");
                exit(0);
            }
        }else if(CommandTypeMap[WriteReq].find(cmd) != CommandTypeMap[WriteReq].end()){
            Request request(WriteReq, requestStr);
            if(clusterClient.Execute(request, resp) < 0){
                printf("err occur\n");
                exit(0);
            }
        }else{
            printf("no such command \n");
        }
        printf("%s\n", resp.GetContent().c_str());
    }

}

