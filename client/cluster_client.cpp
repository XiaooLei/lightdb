//
// Created by xiaolei on 2022/4/13.
//

#include "cluster_client.h"
#define RedirectCode 101

namespace lightdb{

    ClusterClient::ClusterClient() {}

    int ClusterClient::Connect(std::string address, int port) {
        int conn_ret;
        conn_ret = this->readClient.Connect(address, port);
        if(conn_ret < 0){
            return conn_ret;
        }
        conn_ret = this->readClient.Connect(address, port);
        if(conn_ret < 0){
            return conn_ret;
        }
        return 0;
    }

    int ClusterClient::Execute(Request request, Response &response) {
        if(request.GetReqType() == ReadReq){
            this->readClient.Execute(request, response);
            if(response.GetRespCode() == RedirectCode){
                this->readClient.Close();
                RaftServer raftServer;
                raftServer.decode(response.GetContent());
                this->readClient.Connect(raftServer.host, raftServer.port);
            }
            this->readClient.Execute(request, response);
        }else if(request.GetReqType() == WriteReq){
            this->writeClient.Execute(request, response);
            if(response.GetRespCode() == RedirectCode){
                this->writeClient.Close();
                RaftServer raftServer;
                raftServer.decode(response.GetContent());
                this->writeClient.Connect(raftServer.host, raftServer.port);
            }
            this->writeClient.Execute(request, response);
        }else {
            //skip
        }
    }

    int ClusterClient::Close() {
        int close_ret;
        close_ret = this->readClient.Close();
        if(close_ret < 0){
            return close_ret;
        }
        close_ret = this->writeClient.Close();
        if(close_ret < 0){
            return close_ret;
        }
        return 0;
    }

}