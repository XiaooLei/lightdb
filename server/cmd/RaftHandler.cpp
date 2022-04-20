//
// Created by xiaolei on 2022/4/12.
//

#include "RaftHandler.h"
#define RedirectCode 101
#define RaftRespCode 102

namespace lightdb{

    RaftHandler::RaftHandler() {

    }

    void RaftHandler::SetLightdbRequestHandler(LightdbRequestHandler* lightdbRequestHandler) {
        this->lightdbRequestHandler = lightdbRequestHandler;
    }

    void RaftHandler::SetRaft(Raft *raft) {
        this->raft = raft;
    }


    std::string RaftHandler::HandleCmd(std::string request, int conn_fd) {
        //反序列化请求
        Request Req;
        Req.Decode(request);
        //printf("request:%s, ReqType:%d, ReqContent:%s\n", request.c_str(), Req.GetReqType(), Req.GetReqContent().c_str());
        if(Req.GetReqType() == ReadReq){
//            if(getRaftState() == Leader) {
//                //redirect;
//                std::string redirectAddr;
//                int redirectPort;
//                int redirectServerNum;
//                GetRedirectServer(Follower, redirectAddr, redirectPort, redirectServerNum);
//                WriteBack(WrapDirectResp(redirectAddr, redirectPort, redirectServerNum), conn_fd);
//                return "";
//            }
            //处理读请求
            this->lightdbRequestHandler->HandleCmd(Req.GetReqContent(), conn_fd);
        }else if(Req.GetReqType() == WriteReq){
            if(getRaftState() == Follower){
                //redirect
                std::string redirectAddr;
                int redirectPort;
                int redirectServerNum;
                GetRedirectServer(Leader, redirectAddr, redirectPort, redirectServerNum);
                WriteBack(WrapDirectResp(redirectAddr, redirectPort, redirectServerNum), conn_fd);
                return "";
            }
            //处理写请求
            printf("AddLog\n");
            this->raft->AddLog(Req.GetReqContent(), conn_fd);
        }else if(Req.GetReqType() == VoteReq || Req.GetReqType() == AppendEntriesReq){
            //处理raft请求
            std::string raftReply;
            this->HandleRaftRequest(Req.GetReqType(), Req.GetReqContent(), raftReply);
            //这里直接写回
            Response resp(RaftRespCode, raftReply);
            std::string serializedResp;
            resp.Encode(serializedResp);
            WriteBack(serializedResp, conn_fd);
        }
        return "";
    }

    RaftState RaftHandler::getRaftState() {
        return this->raft->getState();
    }

    void RaftHandler::GetRedirectServer(RaftState redirectTo, std::string& redirectToAddress, int& redirectToPort, int& redirectToServerNum){
        if(redirectTo == Leader){
            this->raft->getLeaderAddressAndPort(redirectToAddress, redirectToPort, redirectToServerNum);
        }else if(redirectTo == Follower){
            this->raft->getFollowerAddressAndPort(redirectToAddress, redirectToPort, redirectToServerNum);
        }else{
            //skip
        }
    }

    std::string RaftHandler::WrapDirectResp(std::string redirectToAddress, int redirectToPort, int redirectToServerNum) {
        RaftServer raftServer(redirectToAddress, redirectToPort, redirectToServerNum);
        std::string jsonResp;
        raftServer.encode(jsonResp);
        return Response::ResponseWrap(RedirectCode, jsonResp);
    }

    std::string RaftHandler::StartConsumeApplyQueue() {
        std::thread consumeApplyQueueThread(
                [&](RaftHandler* raftHandler){
                    for(;;) {
                        LogEntry logEntry;
                        raftHandler->raft->ApplyQueue.Dequeue(logEntry);
                        raftHandler->lightdbRequestHandler->HandleCmd(logEntry.Command, logEntry.conn_fd);
                    }
                },this);
        consumeApplyQueueThread.detach();
    }

    void RaftHandler::HandleRaftRequest(RequestType reqType, std::string args, std::string &reply) {
        if(reqType == VoteReq){
            RequestVoteArgs requestVoteArgs;
            requestVoteArgs.decode(args);
            RequestVoteReply requestVoteReply;
            raft->RequestVote(requestVoteArgs, requestVoteReply);
            requestVoteReply.Encode(reply);
        }else if(reqType == AppendEntriesReq){
            printf("AppendEntriesReq:%s \n", args.c_str());
            AppendEntriesArgs appendEntriesArgs;
            appendEntriesArgs.decode(args);
            AppendEntriesReply appendEntriesReply;
            raft->AppendEntries(appendEntriesArgs, appendEntriesReply);
            appendEntriesReply.encode(reply);
        }
    }

    void RaftHandler::WriteBack(const std::string resp, const int &conn_fd) {
        write(conn_fd, resp.c_str(), resp.size());
    }

}
