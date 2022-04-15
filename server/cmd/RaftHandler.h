//
// Created by xiaolei on 2022/4/12.
//

#ifndef MYPROJECT_RAFTHANDLER_H
#define MYPROJECT_RAFTHANDLER_H


#include "LightdbRequestHandler.h"
#include "requestHandler.h"
#include "Response.h"
#include "Request.h"
#include "../../raft/raft.h"

namespace lightdb {
class RaftHandler : public RequestHandler{
private:
    LightdbRequestHandler* lightdbRequestHandler;
    Raft* raft;

public:

    RaftHandler();

    void SetLightdbRequestHandler(LightdbRequestHandler* lightdbRequestHandler1);

    void SetRaft(Raft* raft);

    std::string HandleCmd(std::string request, int conn_fd) override;

    RaftState getRaftState();

    void GetRedirectServer(RaftState redirectTo, std::string& redirectToAddress, int& redirectToPort, int& redirectToServerNum);

    std::string WrapDirectResp(std::string redirectToAddress, int redirectToPort, int redirectToServerNum);

    std::string StartConsumeApplyQueue();

    void HandleRaftRequest(RequestType, std::string args, std::string& reply);

    void WriteBack(const std::string resp, const int& conn_fd);

};


}


#endif //MYPROJECT_RAFTHANDLER_H
