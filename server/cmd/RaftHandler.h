//
// Created by xiaolei on 2022/4/12.
//

#ifndef MYPROJECT_RAFTHANDLER_H
#define MYPROJECT_RAFTHANDLER_H


#include "LightdbRequestHandler.h"
#include "requestHandler.h"
#include "../../raft/raft.h"

namespace lightdb {
class RaftHandler : RequestHandler{
private:
    LightdbRequestHandler* lightdbRequestHandler;
    Raft* raft;

    std::string HandleCmd(std::string request, int conn_fd) override;

    RaftState getRaftState();

    void GetRedirectServer(RaftState redirectTo, std::string& redirectToAddress, int& redirectToPort, int& redirectToServerNum);

    std::string WrapDirectResp(std::string redirectToAddress, int redirectToPort, int redirectToServerNum);




};


}


#endif //MYPROJECT_RAFTHANDLER_H
