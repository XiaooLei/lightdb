//
// Created by 9 on 2022/4/1.
//

#ifndef MYPROJECT_RAFTDATASTRUCTURES_H
#define MYPROJECT_RAFTDATASTRUCTURES_H
#include "../util/cJson/cJSON.h"
#include <string>
#include <vector>
#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <unistd.h>

namespace lightdb{

typedef struct logEntry{
    int Term;
    std::string Command;
    int conn_fd;


    //LogEntry(const int& Term, const std::string& Command, const int& conn_fd):Term(Term), Command(Command), conn_fd(conn_fd){}

    void encode(std::string& str){
        cJSON* cJson = cJSON_CreateObject();
        cJSON_AddNumberToObject(cJson, "Term", Term);
        cJSON_AddStringToObject(cJson, "Command", Command.c_str());
        str = cJSON_Print(cJson);
    };
    void decode(std::string& bytes){
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        this->Term = cJSON_GetObjectItem(cJson, "Term")->valueint;
        this->Command = cJSON_GetObjectItem(cJson, "Command")->valuestring;
    }
}LogEntry;


typedef struct ApplyMsg{
    bool CommandValid;
    std::string Command;
    int CommandIndex;
};


typedef struct requestVoteArgs{
    int Term;
    int CandidateId;
    int LastLogIndex;
    int LastLogTerm;

    void encode(std::string& bytes){
        cJSON* cJson = cJSON_CreateObject();
        cJSON_AddNumberToObject(cJson, "Term", Term);
        cJSON_AddNumberToObject(cJson, "CandidateId", CandidateId);
        cJSON_AddNumberToObject(cJson, "LastLogTerm", LastLogTerm);
        cJSON_AddNumberToObject(cJson, "LastLogIndex", LastLogIndex);
        bytes = cJSON_Print(cJson);
    }

    void decode(std::string bytes){
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        this->Term = cJSON_GetObjectItem(cJson, "Term")->valueint;
        this->CandidateId = cJSON_GetObjectItem(cJson, "CandidateId")->valueint;
        this->LastLogIndex = cJSON_GetObjectItem(cJson, "LastLogIndex")->valueint;
        this->LastLogTerm = cJSON_GetObjectItem(cJson, "LastLogTerm")->valueint;
    }

} RequestVoteArgs;

typedef struct requestVoteRely{
    int Term;
    bool VoteGranted;

    void Encode(std::string& bytes){
        cJSON* cJson = cJSON_CreateObject();
        cJSON_AddNumberToObject(cJson, "Term", Term);
        cJSON_AddBoolToObject(cJson, "VoteGranted", VoteGranted);
        bytes = cJSON_Print(cJson);
    }

    void Decode(std::string bytes) {
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        this->Term = cJSON_GetObjectItem(cJson, "Term")->valueint;
        this->VoteGranted = cJSON_GetObjectItem(cJson, "VoteGranted")->valueint;
    }

}RequestVoteReply;


typedef struct appendEntriesArgs{
    int Term;
    int LeaderId;
    int PrevLogIndex;
    int PrevLogTerm;
    int LeaderCommit;
    std::vector<LogEntry> Entries;

    void encode(std::string& bytes){
        cJSON* cJson = cJSON_CreateObject();
        cJSON_AddNumberToObject(cJson, "Term", Term);
        cJSON_AddNumberToObject(cJson, "LeaderId", LeaderId);
        cJSON_AddNumberToObject(cJson, "PrevLogIndex", PrevLogIndex);
        cJSON_AddNumberToObject(cJson, "PrevLogTerm", PrevLogTerm);
        cJSON_AddNumberToObject(cJson, "LeaderCommit", LeaderCommit);

        cJSON* entries_json = NULL;
        cJSON_AddItemToObject(cJson, "Entries", entries_json = cJSON_CreateArray());
        for(LogEntry entry : Entries){
            std::string entry_bytes;
            entry.encode(entry_bytes);
            cJSON* entry_json = cJSON_Parse(entry_bytes.c_str());
            cJSON_AddItemToArray(entries_json, entry_json);
        }
        bytes = cJSON_Print(cJson);
    }

    void decode(std::string bytes){
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        this->Term = cJSON_GetObjectItem(cJson, "Term")->valueint;
        this->LeaderId = cJSON_GetObjectItem(cJson, "LeaderId")->valueint;
        this->PrevLogIndex = cJSON_GetObjectItem(cJson, "PrevLogIndex")->valueint;
        this->PrevLogTerm = cJSON_GetObjectItem(cJson, "PrevLogTerm")->valueint;
        this->LeaderCommit = cJSON_GetObjectItem(cJson, "LeaderCommit")->valueint;
        cJSON* entries_json = cJSON_GetObjectItem(cJson, "Entries");
        int size = cJSON_GetArraySize(entries_json);
        for(int i = 0; i < size; i++){
            LogEntry logEntry;
            cJSON* entry_json = cJSON_GetArrayItem(entries_json, i);
            std::string entry_bytes = cJSON_Print(entry_json);
            logEntry.decode(entry_bytes);
            Entries.push_back(logEntry);
        }
    }

}AppendEntriesArgs;

typedef struct RaftServer{
    std::string host;
    int port;
    int num;

    RaftServer(){}

    RaftServer(const std::string& host, const int& port, const int& num):host(host),port(port),num(num){}

    void encode(std::string& bytes){
        cJSON* cJson = cJSON_CreateObject();
        cJSON_AddStringToObject(cJson, "host", host.c_str());
        cJSON_AddNumberToObject(cJson, "port", port);
        cJSON_AddNumberToObject(cJson, "num", num);
        bytes = cJSON_Print(cJson);
    }

    void decode(std::string bytes){
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        this->host = cJSON_GetObjectItem(cJson, "host")->valuestring;
        this->port = cJSON_GetObjectItem(cJson, "port")->valueint;
        this->num = cJSON_GetObjectItem(cJson, "num")->valueint;
    }
};


typedef struct appendEntriesReply{
    int Term;
    bool Success;
    int ConflictIndex;
    int ConflictTerm;

    void encode(std::string& bytes){
        cJSON* cJson = cJSON_CreateObject();
        cJSON_AddNumberToObject(cJson, "Term", Term);
        cJSON_AddBoolToObject(cJson, "Success", Success);
        cJSON_AddNumberToObject(cJson, "ConflictIndex", ConflictIndex);
        cJSON_AddNumberToObject(cJson, "ConflictTerm", ConflictTerm);
        bytes = cJSON_Print(cJson);
    }

    void decode(std::string bytes){
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        this->Term = cJSON_GetObjectItem(cJson, "Term")->valueint;
        this->Success = cJSON_GetObjectItem(cJson, "Success")->valueint;
        this->ConflictIndex = cJSON_GetObjectItem(cJson, "ConflictIndex")->valueint;
        this->ConflictTerm = cJSON_GetObjectItem(cJson, "ConflictTerm")->valueint;
    }

}AppendEntriesReply;


typedef struct raftPersist{
    int currentTerm;
    int votedFor;
    std::vector<LogEntry> logs;

    void Encode(std::string& bytes){
        cJSON* cJson = cJSON_CreateObject();
        cJSON_AddNumberToObject(cJson, "CurrentTerm", currentTerm);
        cJSON_AddNumberToObject(cJson, "VotedFor", votedFor);
        cJSON* entries_json = NULL;
        cJSON_AddItemToObject(cJson, "Logs", entries_json = cJSON_CreateArray());
        for(auto entry : logs){
            std::string entry_bytes;
            entry.encode(entry_bytes);
            cJSON* entry_json = cJSON_Parse(entry_bytes.c_str());
            cJSON_AddItemToArray(entries_json, entry_json);
        }
        bytes = cJSON_Print(cJson);
    }


    void Decode(std::string bytes){
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        this->currentTerm = cJSON_GetObjectItem(cJson, "CurrentTerm")->valueint;
        this->votedFor = cJSON_GetObjectItem(cJson, "VotedFor")->valueint;
        cJSON* entries_json = cJSON_GetObjectItem(cJson, "Logs");
        int size = cJSON_GetArraySize(entries_json);
        for(int i = 0; i < size; i++){
            LogEntry logEntry;
            cJSON* entry_json = cJSON_GetArrayItem(entries_json, i);
            std::string entry_bytes = cJSON_Print(entry_json);
            logEntry.decode(entry_bytes);
            logs.push_back(logEntry);
        }
    }


    int persist(std::string persistPath){
        std::string bytes;
        Encode(bytes);
        std::ofstream out(persistPath);
        out<<bytes;
        return 0;
    }

    int readFromPesist(std::string persistPath){
        std::ifstream t(persistPath);
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string raftConfig(buffer.str());
        Decode(raftConfig);
    }

}RaftPersist;


}

#endif //MYPROJECT_RAFTDATASTRUCTURES_H
