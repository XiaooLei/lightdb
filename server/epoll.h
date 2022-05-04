//
// Created by xiaolei on 2022/3/10.
//

#ifndef MYPROJECT_EPOLL_H
#define MYPROJECT_EPOLL_H

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../lightdb.h"
#include "cmd/requestHandler.h"

#define MAX_CONN 200
#define MAX_POLL 10000
#define MAX_LINE 1000

namespace lightdb{

class Epoll{

private:
    RequestHandler* requestHandler;

    int epollFd;
    int _listen_fd;
    struct epoll_event evs[MAX_POLL];
    int cur_fds = 0;// 当前epoll监听的fd数目

public:

    explicit Epoll(int listen_fd);

    void SetRequestHandler(RequestHandler* requestHandler);

    void EpollWait();

    void AddConn(int conn_fd);

    void CloseAndDel(int fd);


};


}// namespace lightdb

#endif //MYPROJECT_EPOLL_H
