//
// Created by xiaolei on 2022/3/23.
//

#ifndef MYPROJECT_CLIENT_H
#define MYPROJECT_CLIENT_H

#define PORT 10000
#define MAXDATASIZE 1000
#include <string>
#include <netdb.h>
#include <cstring>
#include "../server/cmd/Response.h"

namespace lightdb{

    class LightdbClient{
    private:
        bool connected;
        int socket_fd;
        struct hostent *he;
        struct sockaddr_in server;
        std::string remoteAddress;
    public:

        ~LightdbClient(){
            if(connected){
                Close();
            }
        }

        int Connect(std::string address, int port);

        int Execute(std::string request, Response& response);

        int Close();

        const char* RemoteAddress();

    };





}





#endif //MYPROJECT_CLIENT_H
