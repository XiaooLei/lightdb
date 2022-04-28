//
// Created by xiaolei on 2022/3/23.
//


#include <unistd.h>
#include <fcntl.h>
#include "lightdb_client.h"

namespace lightdb{
    enum CommandType{Read, Write, RequestVote, AppendEntries};

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
            {RequestVote,{"request_vote"}},
            {AppendEntries,{"append_entries"}}
            };

    int LightdbClient::Connect(std::string address, int port) {
        if((he=gethostbyname(address.c_str()))==NULL)
        {
            printf("gethostbyname() error\n");
            exit(1);
        }
        if((socket_fd=socket(AF_INET,SOCK_STREAM, 0))==-1)
        {
            printf("socket() error\n");
            exit(1);
        }
        bzero(&server,sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr = *((struct in_addr *)he->h_addr);
        if(connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
        {
            printf("connect() error\n");
            return -1;
        }
        remoteAddress = address;
        connected = true;
        //设置成非阻塞
        //fcntl(socket_fd, F_SETFL,  O_NONBLOCK);
        return 0;
    }


    int LightdbClient::Execute(Request request, Response& response) {
        //todo 先直接返回-1吧
        if(!connected){
            return -1;
        }
        int num = 0;
        std::string request_sequence;
        request.Encode(request_sequence);
        if((num=send(socket_fd, request_sequence.c_str(), request_sequence.size(), 0))==-1){
            printf("send() error\n");
            return -1;
        }
        struct timeval timeout={2,0};//3s
        int ret=setsockopt(socket_fd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
        if(ret < 0){
            printf("timeout set failed\n");
        }
        char buf[MAXDATASIZE];
        if((num=recv(socket_fd, buf, MAXDATASIZE,0))==-1)
        {
            if(errno==EAGAIN){
                printf("timeout\n");
                return -2;
            }
            printf("recv() error\n");
            return -1;
        }
        buf[num]='\0';
        std::string respJson(buf);
        //printf("resp:%s \n", respJson.c_str());
        if(response.Decode(respJson) < 0){
            return -1;
        }
        return 0;
    }

    int LightdbClient::Close() {
        if(!connected){
            return -1;
        }
        close(socket_fd);
        return 0;
    }

    const char* LightdbClient::RemoteAddress() {
        return remoteAddress.c_str();
    }

}
