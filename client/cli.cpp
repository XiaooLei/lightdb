//
// Created by xiaolei on 2022/3/23.
//

#define MAX_REQUEST_LENGTH 1000
#include "lightdb_client.h"
#include <vector>
#include "../util/str.h"
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
    LightdbClient lightdbClient;
    if( argc != 3){
        printf("wrong args count \n");
        exit(0);
    }
    char* address = argv[1];
    int port = atoi(argv[2]);

    if(lightdbClient.Connect(address, port) < 0){
        printf("connect failed");
        exit(0);
    }
    printf("successfully connected to lighted, address:%s \n", lightdbClient.RemoteAddress());
    while(true){
        printf("(%s:%d)>", lightdbClient.RemoteAddress(), port);
        std::string requestStr;
        std::getline(std::cin, requestStr);
        if(requestStr.size() == 0){
            continue;
        }
        //printf("request:%s||", request.c_str());
        if(requestStr.compare("quit") == 0){
            lightdbClient.Close();
            exit(0);
        }

        Response resp;
        std::vector<std::string> cmdAndArgs;
        splitStrBySpace(requestStr, cmdAndArgs);
        if(cmdAndArgs.size() == 0){
            return -1;
        }
        std::string cmd = cmdAndArgs[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if(CommandTypeMap[ReadReq].find(cmd) != CommandTypeMap[ReadReq].end()){
            Request request(ReadReq, requestStr);
            if(lightdbClient.Execute(request, resp) < 0){
                printf("err occur\n");
                exit(0);
            }
        }else if(CommandTypeMap[WriteReq].find(cmd) != CommandTypeMap[WriteReq].end()){
            Request request(WriteReq, requestStr);
            if(lightdbClient.Execute(request, resp) < 0){
                printf("err occur\n");
                exit(0);
            }
        }else{
            printf("no such command \n");
        }
        printf("%s\n", resp.GetContent().c_str());
    }

}

