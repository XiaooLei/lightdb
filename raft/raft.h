//
// Created by xiaolei on 2022/3/30.
//

#ifndef MYPROJECT_RAFT_H
#define MYPROJECT_RAFT_H

#include <mutex>
#include "raftDataStructures.h"
#include <thread>
#include <chrono>
#include "../util/dir_util.h"
#include <condition_variable>
#include <fstream>
#include "../lightdb.h"
#include <unistd.h>
#include "rpcClient.h"
#include "../sync/SafeQueue.h"

namespace lightdb {

enum RaftState{Leader, Candidate, Follower};

class Raft {

private:
    std::string raftConfigPath;
    std::string raftPersistDir;

    std::recursive_mutex mtx;

    std::vector<RpcClient> peers;

    int leader;

    int me;

    //持久化变量
    int currentTerm;
    int votedFor;
    std::vector<LogEntry> logs;

    //volatile state on all servers;
    int commitIndex;
    int lastApplied;

    //volatile on leaders;
    std::vector<int> nextIndex;
    std::vector<int> matchIndex;

    // other auxiliary states
    RaftState state;
    int voteCount;

    //todo 这里是否需要一个条件变量？
    std::condition_variable_any appendEntriesCond;
    bool leaderOut;

    //todo 用于协调是否发起选举
    std::mutex reelectMtx;
    std::condition_variable_any reelectCond;
    bool reElect = true;

    //todo 用于协调是否赢得选举
    std::condition_variable_any winElectionCond;
    bool winElection;

public:

    SafeQueue<LogEntry> ApplyQueue;

public:

    Raft(std::string raftPersistDir, std::string raftConfigPath):raftPersistDir(raftPersistDir),raftConfigPath(raftConfigPath){
        //read rpcClients from raft.json
        readRpcClients(raftConfigPath);
        //initialize
        setCurState(Follower);
        setCurTerm(0);
        setVotedFor(-1);
        setVoteCount(0);
        setCommitIndex(0);
        setCommitIndex(0);
        setLastApplyed(0);
        logs.push_back(LogEntry{0, "", 0});

        //readPersist
        InitialRaftPersist();
        //todo 先删除readPersist();
        setCurTerm(0);
        setVotedFor(-1);


        nextIndex = std::vector<int>(peers.size(), 1);
        matchIndex = std::vector<int>(peers.size(), 0);


        std::thread raftThread(
                [&, this](Raft* raft){
                        raft->RunServer();
                    }, this);
        raftThread.detach();
    }

    //从raft.json文件中读取集群中的各个server
    void readRpcClients(std::string raftConfigPath){
        std::ifstream t(raftConfigPath);
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string raftConfig(buffer.str());
        cJSON* raftConfig_json = cJSON_Parse(raftConfig.c_str());
        cJSON* servers_json = cJSON_GetObjectItem(raftConfig_json, "servers");
        //me = cJSON_GetObjectItem(raftConfig_json, "me")->valueint;
        int size = cJSON_GetArraySize(servers_json);
        for(int i = 0; i < size; i++){
            RaftServer raftServer;
            cJSON* raftServerJson = cJSON_GetArrayItem(servers_json, i);
            std::string raftServerBytes = cJSON_Print(raftServerJson);
            raftServer.decode(raftServerBytes);
            RpcClient client(raftServer.host, raftServer.port, raftServer.num);
            this->peers.push_back(client);
        }
    }

    RaftState getState(){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        return state;
    }

    void getLeaderAddressAndPort(std::string& address, int& port, int& serverNum){
        address = this->peers[leader].server_end_address;
        port = this->peers[leader].server_end_port;
        serverNum = this->leader;
    }

    void getFollowerAddressAndPort(std::string& address, int& port, int& serverNum){
        int follower = rand()%this->peers.size();
        if(follower == leader){
            follower = (follower + 1)%this->peers.size();
        }
        address = this->peers[follower].server_end_address;
        port = this->peers[follower].server_end_port;
        serverNum = follower;
    }

    /**************************************         这部分用来设置共享变量          *******************************/

    int getCurTerm(){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        return currentTerm;
    }

    void setCurTerm(int term){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        this->currentTerm = term;
    }

    RaftState getCurState(){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        return state;
    }

    void setCurState(RaftState state){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        this->state = state;
    }

    int getVoteCount(){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        return voteCount;
    }

    void setVoteCount(int voteCount){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        this->voteCount = voteCount;
    }

    int getVotedFor(){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        return votedFor;
    }

    void setVotedFor(int votedFor){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        this->votedFor = votedFor;
    }

    int getCommitIndex(){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        return commitIndex;
    }

    void setCommitIndex(int commitIndex){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        this->commitIndex = commitIndex;
    }

    int getLastApplied(){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        return lastApplied;
    }

    void setLastApplyed(int lastApplied){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        this->lastApplied = lastApplied;
    }

    bool getReelect(){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        return this->reElect;
    }

    void setReelect(bool reelect){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        this->reElect = reelect;
    }

    bool getLeaderOut(){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        return this->leaderOut;
    }
    void setLeaderOut(bool leaderOut){
        std::lock_guard<std::recursive_mutex> lock(mtx);
        this->leaderOut = leaderOut;
    }


    void persist(){
        RaftPersist raftPersist = {getCurTerm(), getVotedFor(), logs};
        raftPersist.persist(raftPersistDir + "/" + "raftPersist.json");
    }

    void InitialRaftPersist(){
        if(access((raftPersistDir + "/" + "raftPersist.json").c_str(), 0) < 0){
            persist();
        }
    }

    void readPersist(){
        RaftPersist raftPersist;
        raftPersist.readFromPesist(raftPersistDir + "/" + "raftPersist.json");
        currentTerm = raftPersist.currentTerm;
        votedFor = raftPersist.votedFor;
        logs = raftPersist.logs;
    }

    int getLastIndex(){
        return logs.size()-1;
    }

    int getLastTerm(){
        return logs[getLastIndex()].Term;
    }

    void ApplyLogs(){
        //todo
        printf("apply logs  lastApplied:%d, commitIndex:%d ||", getLastApplied(), getCommitIndex());
        DEBUGPrint();
        for(int i = getLastApplied() + 1; i <= getCommitIndex(); i++){
            ApplyQueue.Enqueue(logs[i]);
            setLastApplyed(i);
        }
    }

    int getElectionTimeOut(){
        return (150 + rand()%150) * 10 * 5;
    }

    void stepDownToFollower(int term){
        printf("step down to follower ||");
        DEBUGPrint();
        setCurState(Follower);
        setCurTerm(term);
        setVotedFor(-1);
    }

    void convertToLeader(){
        if(getCurState() != Candidate){
            return;
        }
        setCurState(Leader);
        int lastIndex = getLastIndex();
        nextIndex = std::vector<int>(peers.size(), lastIndex);
        matchIndex = std::vector<int>(peers.size(), 0);
        //broadcastAppendEntries
        broadcastAppendEntries();
    }

    void convertToCandidate(RaftState fromState){
        printf("convert to candidate ||");
        DEBUGPrint();
        if(getCurState() != fromState){
            printf("convert to candidate failed ||");
            DEBUGPrint();
            return;
        }
        setCurState(Candidate);
        setCurTerm(getCurTerm() + 1);
        setVotedFor(me);
        //todo 测试方便直接先从1开始
        setVoteCount(1);
        persist();
        std::thread broadcastRequestVoteThread([](Raft* raft) {
                                                   raft->broadcastRequestVote();
                                               },this);
        broadcastRequestVoteThread.detach();
    }

    bool isLogUpToDate(int cLastIndex, int cLastTerm){
        printf("isLogUpToDate, cLastIndex:%d, cLastTerm:%d, myLastIndex:%d, myLastTerm:%d ||",
               cLastIndex, cLastTerm, getLastIndex(), getLastTerm());
        DEBUGPrint();
        int myLastIndex = getLastIndex();
        int myLastTerm = getLastTerm();
        if(cLastTerm == myLastTerm){
            return cLastIndex >= myLastIndex;
        }
        return cLastTerm > myLastTerm;
    }


    void DEBUGPrint(){
        printf("[DEBUG]CurrentTimeStamp:%lu, Term:%d State:%d,VotedFor:%d, VoteCount:%d logSize:%d\n\n",
               (getCurrentTimeStamp())%1000000, getCurTerm(), getCurState(), getVotedFor(), getVoteCount(), this->logs.size());
    }

    void RequestVote(RequestVoteArgs args, RequestVoteReply& reply){
        printf("vote request from server %d ||", args.CandidateId);
        DEBUGPrint();
        if(args.Term < getCurTerm()){
            reply.Term = getCurTerm();
            reply.VoteGranted = false;
            persist();
            return;
        }

        if(args.Term > getCurTerm()){
            stepDownToFollower(args.Term);
            printf("notify_all ||");
            DEBUGPrint();
            appendEntriesCond.notify_all();
            setLeaderOut(true);
        }

        reply.Term = getCurTerm();
        reply.VoteGranted = false;

        bool logUptoDate = isLogUpToDate(args.LastLogIndex, args.LastLogTerm);
        printf("logUptoDate :%d ||", logUptoDate);
        DEBUGPrint();
        if( (getVotedFor() == -1 || getVotedFor() == args.CandidateId) &&
                logUptoDate){
            reply.VoteGranted = true;
            setVotedFor(args.CandidateId);
            setReelect(false);
            reelectCond.notify_all();
            //todo
        }
        persist();
    }

    void sendRequestVote(int server, RequestVoteArgs& args, RequestVoteReply & reply){
        if(peers[server].sendRequestVote(args, reply) < 0){
            return;
        }
        if(getCurState() != Candidate || args.Term != getCurTerm() || reply.Term < getCurTerm()){
            return;
        }

        if(reply.Term > getCurTerm()){
            stepDownToFollower(reply.Term);
            persist();
            return;
        }

        if(reply.VoteGranted){
            setVoteCount(getVoteCount() + 1);
            if(getVoteCount() == peers.size()/2 +1){
                //todo convert to leader
                winElection = true;
                printf("win election, become an Leader ||");
                DEBUGPrint();
                setCurState(Leader);
                reelectCond.notify_all();
            }
        }
    }

    void broadcastRequestVote(){
        if(getCurState() != Candidate){
            return;
        }
        RequestVoteArgs args = {getCurTerm(), me, getLastIndex(), getLastTerm()};
        for(int server = 0; server < peers.size(); server++){
            if(server == me){
                //如果是自己，同样要检查是否选举成功
                if(getVoteCount() == peers.size()/2 +1){
                    //todo convert to leader
                    winElection = true;
                    printf("win election, become an Leader ||");
                    DEBUGPrint();
                    setCurState(Leader);
                    reelectCond.notify_all();
                }
                continue;
            }
            RequestVoteReply reply;
//            if(server != me){
//                std::thread voteThread([&, this](Raft* raft){raft->sendRequestVote(server, args, reply);}, this);
//                voteThread.detach();
//            }
            sendRequestVote(server, args, reply);
        }
    }

    void broadcastAppendEntries(){
        printf("broadcastAppendEntries ||");
        DEBUGPrint();
        if(getCurState() != Leader){
            return;
        }
        for(int server = 0; server < peers.size(); server++){
            //todo 暂时对所有节点都处理，包括自己
            if(server == me){
                CheckCommit();
            }
            if(server != me){
                AppendEntriesArgs args;
                {
                    std::lock_guard<std::recursive_mutex> lock(mtx);
                    args.Term = getCurTerm();
                    args.LeaderId = me;
                    args.PrevLogIndex = nextIndex[server] - 1;
                    args.PrevLogTerm = logs[args.PrevLogIndex].Term;
                    args.LeaderCommit = getCommitIndex();
                    for (int i = args.PrevLogIndex + 1; i < logs.size(); i++) {
                        args.Entries.push_back(logs[i]);
                    }
                }
                AppendEntriesReply reply;
//                std::thread sendAppendEntriesThread([&, this](Raft* raft){
//                    raft->sendAppendEntries(server, args, reply);
//                }, this);
//                sendAppendEntriesThread.detach();
                //todo 先用串行
                this->sendAppendEntries(server, args, reply);
            }
        }
    }

    void AppendEntries(AppendEntriesArgs& args, AppendEntriesReply& reply){
        printf("handle append entries request ||");
        DEBUGPrint();
        if(args.Term < getCurTerm()){
            reply.Term = getCurTerm();
            reply.ConflictIndex = -1;
            reply.ConflictTerm = -1;
            reply.Success = false;
            persist();
            return;
        }

        if(args.Term > getCurTerm()){
            stepDownToFollower(args.Term);
        }
        printf("get heartbeat from leader ||");
        DEBUGPrint();
        setReelect(false);
        reelectCond.notify_all();

        int lastIndex = getLastIndex();
        //todo
        reply.Term = getCurTerm();
        reply.Success = false;
        reply.ConflictTerm = -1;
        reply.ConflictIndex = -1;

        //follower log is shorter than leader
        if(args.PrevLogIndex > lastIndex){
            reply.ConflictIndex = lastIndex + 1;
            return;
        }

        //log Consistency check fails, different term at prevLogIndex
        //printf("logs size:%d, args.PrevLogIndex%d \n", logs.size(), args.PrevLogIndex);
        int cfTerm = logs[args.PrevLogIndex].Term;
        if(cfTerm!=args.PrevLogTerm) {
            reply.ConflictTerm = cfTerm;
            for (int i = args.PrevLogIndex; i>=0 && logs[i].Term == cfTerm; i--){
                reply.ConflictIndex = i;
            }
            reply.Success = false;
            persist();
            return;
        }

        // truncate log if an existing entry conflicts with a new one
        int i = args.PrevLogIndex + 1; //logs index
        int j = 0; //args.entries index
        for(; i < getLastIndex() + 1 && j < args.Entries.size();){
            if(logs[i].Term != args.Entries[j].Term){
                break;
            }
            i++;
            j++;
        }
        printf("i:%d, j:%d, logs.size:%d , args.entries.size:%d \n", i, j, logs.size(), args.Entries.size());
        //i:1, j:0, logs.size:1 , args.entries.size:0
        //i:1, j:2, logs.size:2 , args.entries.size:1
        DEBUGPrint();
        logs = std::vector<LogEntry>(logs.begin(), logs.begin() + i);
        args.Entries = std::vector<LogEntry>(args.Entries.begin() + j, args.Entries.end());
        for(auto e : args.Entries){
            logs.push_back(e);
        }
        reply.Success = true;
        //update commit index to min(leader commit, lastIndex)
        if(args.LeaderCommit > getCommitIndex()){
            int lastIndex = getLastIndex();
            if(args.LeaderCommit < lastIndex){
                setCommitIndex(args.LeaderCommit);
            }else{
                setCommitIndex(lastIndex);
            }
            //todo ApplyLogs
            ApplyLogs();
        }

    }

    void sendAppendEntries(int server, AppendEntriesArgs& args, AppendEntriesReply& reply){
        if(this->peers[server].sendAppendEntries(args, reply) < 0) {
            return;
        }
        if(getCurState() != Leader || args.Term != getCurTerm() || reply.Term < getCurTerm()){
            persist();
            return;
        }
        if(reply.Term > getCurTerm()){
            stepDownToFollower(reply.Term);
            persist();
            return;
        }
        //update match index and nextIndex of the follower;
        if(reply.Success){
            int newMatchIndex = args.PrevLogIndex + args.Entries.size();
            if(newMatchIndex > matchIndex[server]){
                matchIndex[server] = newMatchIndex;
            }
            nextIndex[server] = matchIndex[server] + 1;
        }else if(reply.ConflictTerm < 0){
            printf("pointA: reply.ConflictIndex:%d ||", reply.ConflictIndex);
            DEBUGPrint();
            // follower's log shorter than leader's
            nextIndex[server] = reply.ConflictIndex;
            matchIndex[server] = nextIndex[server] - 1;
        }else{
            // try to find the conflictTerm in log
            int newNextIndex = getLastIndex();
            for(; newNextIndex >= 0; newNextIndex--){
                if(logs[newNextIndex].Term == reply.ConflictTerm){
                    break;
                }
            }
            // if not found, set nextIndex to conflictIndex
            if(newNextIndex < 0){
                nextIndex[server] = reply.ConflictIndex;
            }else{
                nextIndex[server] = newNextIndex;
            }
            matchIndex[server] = nextIndex[server] - 1;
        }
        CheckCommit();
    }

    void CheckCommit(){
        // if there exists an N such that N > commitIndex, a majority of
        // matchIndex[i] >= N, and log[N].term == currentTerm, set commitIndex = N
        for(int n = getLastIndex(); n >= getCommitIndex(); n--){
            int count = 1;
            if(logs[n].Term == getCurTerm()){
                for(int i = 0; i < peers.size(); i++){
                    if(i != this->me && matchIndex[i] >= n){
                        count++;
                    }
                }
            }
            if(count > peers.size()/2 ){
                setCommitIndex(n);
                std::thread applyThread([&, this](Raft* raft){raft->ApplyLogs();}, this);
                applyThread.detach();
                break;
            }
        }
        persist();
    }

    void AddLog(const std::string& command, const int& conn_fd){
        //todo 加入日志集合
        std::lock_guard<std::recursive_mutex> lock(mtx);
        this->logs.push_back({this->getCurTerm(), command, conn_fd});
    }

    void RunServer(){
        printf("raft server started \n");
        for(;;){
            printf("new loop ||");
            DEBUGPrint();
            switch(getCurState()){
                case Leader:{
                    while(true){
                        appendEntriesCond.wait_for(reelectMtx, chrono::milliseconds (1200 * 5));
                        if(getLeaderOut()){
                            break;
                        }else{
                            broadcastAppendEntries();
                            break;
                        }
                    }
                    break;
                }
                case Candidate:{
                    while(true){
                        reelectCond.wait_for(reelectMtx, chrono::milliseconds (getElectionTimeOut()));
                        if(winElection){
                            convertToLeader();
                            break;
                        }else{
                            convertToCandidate(Candidate);
                            break;
                        }
                    }
                    break;
                }
                case Follower:{
                    while(true) {
                        int electionTimeOut = getElectionTimeOut();
                        reelectCond.wait_for(reelectMtx, chrono::milliseconds (electionTimeOut));
                        printf("FollowerWait_for out, reelect:%d ||", getReelect());
                        DEBUGPrint();
                        if (getReelect()) {
                            convertToCandidate(Follower);
                            break;
                        } else {
                            setReelect(true);
                            continue;
                        }
                    }
                    break;
                }
            }

        }
    }


};

}






#endif //MYPROJECT_RAFT_H
