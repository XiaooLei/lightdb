//
// Created by xiaolei on 2022/3/10.
//


#include "server.h"
namespace lightdb{

    const int QUEUE_SIZE = 20;

    void Server::SetRequestHandler(RequestHandler* requestHandler){
        this->_requestHandler = requestHandler;
    }


    int Server::Listen(int port){
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(server_fd == -1){
            std::cout << "create socket err" << std::endl;
            exit(EXIT_FAILURE);
        }
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons( port );
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        //绑定地址
        if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) == -1)
        {
            std::cout << "bind socket err" << std::endl;
            exit(EXIT_FAILURE);
        }
        //监听地址
        if (listen(server_fd, QUEUE_SIZE) == -1)
        {
            std::cout << "listen socket err" << std::endl;
            exit(EXIT_FAILURE);
        }
        printf("[INFO]start to listen on port %d \n", port);
        //todo
        return 0;
    }

    int Server::Loop() {
        //todo
        epoll = new Epoll(server_fd);
        epoll->SetRequestHandler(_requestHandler);
        while(true){
            epoll->EpollWait();
        }
    }


}//namespace lightdb
