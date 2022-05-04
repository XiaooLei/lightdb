//
// Created by xiaolei on 2022/3/30.
//

#ifndef MYPROJECT_RPCCLIENT_H
#define MYPROJECT_RPCCLIENT_H
#include "../client/lightdb_client.h"
#include "raftDataStructures.h"
namespace lightdb {

class RpcClient{
public:
    LightdbClient* lightdb_client;
    std::string server_end_address;
    int server_end_port;
    int num;


    RpcClient(std::string serverHost, int serverPort,int num):server_end_address(serverHost), server_end_port(serverPort), num(num){
        this->lightdb_client = new LightdbClient();
    }
    RpcClient():lightdb_client(nullptr), server_end_port(0), server_end_address(""), num(0){};

    int sendRequestVote(const RequestVoteArgs& args, RequestVoteReply& reply){
        std::string args_bytes;
        args.encode(args_bytes);
        Request voteRequest(VoteReq, args_bytes);
        bool isnull;
        isnull = this->lightdb_client == nullptr;
        if(isnull){
            return -3;
        }
        if(lightdb_client->Connect(server_end_address, server_end_port) < 0){
            return -1;
        }
        Response response;
        if(lightdb_client->Execute(voteRequest, response) < 0){
            return -1;
        }
        printf("vote response:%s \n", response.GetContent().c_str());
        reply.Decode(response.GetContent());
        //短连接请求
        lightdb_client->Close();
        return 0;
    }

    int sendAppendEntries(const AppendEntriesArgs& args, AppendEntriesReply& reply){
        std::string args_byte;
        args.encode(args_byte);
        if(lightdb_client->Connect(server_end_address, server_end_port) < 0){
            return -1;
        }

        Response response;
        if(lightdb_client->Execute(Request(AppendEntriesReq, args_byte), response) < 0){
            return -1;
        }
        reply.decode(response.GetContent());
        //短连接请求
        lightdb_client->Close();
        return 0;
    }

};

}

#endif //MYPROJECT_RPCCLIENT_H
