//
// Created by xiaolei on 2022/3/10.
//

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <linux/in.h>
#include "epoll.h"
namespace lightdb{

    Epoll::Epoll(int listen_fd): requestHandler(nullptr), _listen_fd(listen_fd){
        epollFd = epoll_create(MAX_CONN);
        if(epollFd == -1){
            printf("Failed to create epoll context.%s", strerror(errno));
            exit(1);
        }
        //注册服务器fd
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT;
        ev.data.fd = listen_fd;
        if(epoll_ctl(epollFd, EPOLL_CTL_ADD, listen_fd, &ev) < 0){
            printf("Epoll Error : %d\n", errno);
            exit( EXIT_FAILURE );
        }
        cur_fds++;
    }

    void Epoll::SetRequestHandler(RequestHandler* requestHandler){
        this->requestHandler = requestHandler;
    }

    void Epoll::EpollWait() {
        int count = epoll_wait(epollFd, evs, cur_fds, -1);
        if(count < 0){
            printf( "Epoll Wait Error : %d\n", errno );
            exit( EXIT_FAILURE );
        }
        struct sockaddr_in cliaddr;
        for(int i = 0; i<count; i++){
            if(evs[i].data.fd == _listen_fd && cur_fds < MAX_POLL){
                int len = sizeof(struct sockaddr_in);
                int conn_fd = accept(_listen_fd, (struct sockaddr*)&cliaddr, reinterpret_cast<socklen_t *>(&len));
                printf( "[INFO][New client connected!]\n");
                AddConn(conn_fd);
            }else{
                char buf[MAX_LINE];
                memset(buf, '\0', sizeof(buf));
                int nread = read(evs[i].data.fd, buf, sizeof(buf));
                if( nread<=0 ){
                    CloseAndDel(evs[i].data.fd);
                    continue;
                }
                if(strncmp(buf, "quit", 4) == 0){
                    printf("close connection \n");
                    CloseAndDel(evs[i].data.fd);
                    continue;
                }
                std::string request;
                request.assign(buf, nread);
                std::string resp = this->requestHandler->HandleCmd(request, evs[i].data.fd);
            }
        }
    }


    void Epoll::AddConn(int conn_fd) {
        struct epoll_event ev;
        ev.events = EPOLLIN;	// accept Read!
        ev.data.fd = conn_fd;
        if(epoll_ctl(epollFd, EPOLL_CTL_ADD, conn_fd, &ev) < 0){
            printf("Epoll Add Error : %d\n", errno);
            exit( EXIT_FAILURE );
        }
        cur_fds++;
   }

    void Epoll::CloseAndDel(int fd) {
        close(fd);
        epoll_event ev;
        epoll_ctl( epollFd, EPOLL_CTL_DEL, fd, &ev);	//删除加入的fd
        --cur_fds;
   }

}

