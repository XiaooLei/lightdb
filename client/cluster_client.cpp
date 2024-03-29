//
// Created by xiaolei on 2022/4/13.
//

#include "cluster_client.h"
#define RedirectCode 101

namespace lightdb{

    ClusterClient::ClusterClient() = default;

    int ClusterClient::Connect(const std::string& address, int port){
        remoteAddress = address;
        this->port = port;
        int conn_ret;
        conn_ret = this->readClient.Connect(address, port);
        if(conn_ret < 0){
            return conn_ret;
        }
        conn_ret = this->writeClient.Connect(address, port);
        if(conn_ret < 0){
            return conn_ret;
        }
        return 0;
    }

    int ClusterClient::Execute(Request request, Response &response) {
        int ret;
        if(request.GetReqType() == ReadReq){
            ret = this->readClient.Execute(request, response);
            if(response.GetRespCode() == RedirectCode){
                this->readClient.Close();
                RaftServer raftServer;
                raftServer.decode(response.GetContent());
                this->readClient.Connect(raftServer.host, raftServer.port);
                ret = this->readClient.Execute(request, response);
            }
        }else if(request.GetReqType() == WriteReq){
            ret = this->writeClient.Execute(request, response);
            if(response.GetRespCode() == RedirectCode){
                this->writeClient.Close();
                RaftServer raftServer;
                raftServer.decode(response.GetContent());
                this->writeClient.Connect(raftServer.host, raftServer.port);
                ret = this->writeClient.Execute(request, response);
            }
        }else {
            //skip
        }
        return ret;
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


    std::string ClusterClient::ReadRemoteAddress() {
        return readClient.RemoteAddress();
    }

    std::string ClusterClient::WriteRemoteAddress() {
        return writeClient.RemoteAddress();
    }

    int ClusterClient::ReadRemotePort() {
        return readClient.RemotePort();
    }

    int ClusterClient::WriteRemotePort() {
        return writeClient.RemotePort();
    }


}