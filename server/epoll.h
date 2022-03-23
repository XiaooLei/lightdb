//
// Created by 9 on 2022/3/10.
//

#ifndef MYPROJECT_EPOLL_H
#define MYPROJECT_EPOLL_H

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../lightdb.h"
#include "cmd/requestHandler.h"

#define MAX_CONN 200
#define MAX_POLL 1000 * 1000
#define MAX_LINE 1000

namespace lightdb{

class Epoll{

private:
    RequestHandler* requestHandler;

    int epollFd;
    int listen_fd;
    struct epoll_event evs[MAX_POLL];
    int cur_fds = 0;

public:

    Epoll(int listen_fd);

    void SetRequestHandler(RequestHandler* requestHandler);

    void EpollWait();

    int AddConn(int conn_fd);

    int DelEvent(int fd);


    int HandleNewConn(int fd);

};


}// namespace lightdb

#endif //MYPROJECT_EPOLL_H
