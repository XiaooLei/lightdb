//
// Created by xiaolei on 2022/3/23.
//


#include <unistd.h>
#include "lightdb_client.h"

namespace lightdb{

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
            exit(1);
        }
        remoteAddress = address;
        connected = true;
        return 0;
    }


    int LightdbClient::Execute(std::string request, std::string &resp) {
        if(!connected){
            return -1;
        }
        int num = 0;
        if((num=send(socket_fd, request.c_str(), request.size(), 0))==-1){
            printf("send() error\n");
            exit(1);
        }
        char buf[MAXDATASIZE];
        if((num=recv(socket_fd, buf, MAXDATASIZE,0))==-1)
        {
            printf("recv() error\n");
            exit(1);
        }
        buf[num]='\0';
        resp.assign(buf);
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
