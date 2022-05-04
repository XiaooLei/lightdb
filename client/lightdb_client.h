//
// Created by xiaolei on 2022/3/23.
//

#ifndef MYPROJECT_CLIENT_H
#define MYPROJECT_CLIENT_H

#define MAXDATASIZE 1000
#include <string>
#include <netdb.h>
#include <cstring>
#include <set>
#include <map>
#include "../server/cmd/Response.h"
#include "../server/cmd/Request.h"

namespace lightdb{

    class LightdbClient{
    private:
        bool connected;
        int socket_fd;
        struct hostent *he;
        struct sockaddr_in server;
        std::string remoteAddress;
        int port;
    public:
        LightdbClient():connected(false), socket_fd(-1), he(nullptr), port(-1){}

        ~LightdbClient(){
            if(connected){
                Close();
            }
        }

        int Connect(std::string address, int port);

        int Execute(Request request, Response& response);

        int Close();

        const char* RemoteAddress();

        int RemotePort();
    };





}





#endif //MYPROJECT_CLIENT_H
