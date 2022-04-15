//
// Created by 9 on 2022/3/10.
//

#include "server.h"
#include "cmd/LightdbRequestHandler.h"
#include "cmd/RaftHandler.h"

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("wrong args count\n");
        exit(0);
    }
    lightdb::Server* server = new lightdb::Server();

    lightdb::LightDB* lightdb = new lightdb::LightDB();
    lightdb::Config config = lightdb::Config::BuildConfig(argv[1]);

    config.InitDirs();
    lightdb->Open(&config, true);

    //处理引擎层
    lightdb::LightdbRequestHandler* requestHandler = new lightdb::LightdbRequestHandler(lightdb);

    //处理数据同步层
    lightdb::RaftHandler* raftHandler = new lightdb::RaftHandler();
    raftHandler->SetLightdbRequestHandler(requestHandler);

    lightdb::Raft* raft = new lightdb::Raft(config.dirPath, argv[2]);
    raftHandler->SetRaft(raft);
    raftHandler->StartConsumeApplyQueue();

    server->SetRequestHandler(raftHandler);

    lightdb::init_all();

    server->Listen(config.port);
    server->Loop();
    delete server;
}
