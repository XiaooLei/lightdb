//
// Created by 9 on 2022/3/10.
//

#include "server.h"
#include "cmd/LightdbRequestHandler.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("wrong args count\n");
        exit(0);
    }
    lightdb::Server* server = new lightdb::Server();

    lightdb::LightDB* lightdb = new lightdb::LightDB();
    lightdb::Config config = lightdb::Config::BuildConfig(argv[1]);
    config.InitDirs();
    lightdb->Open(&config, true);

    lightdb::RequestHandler* requestHandler = new lightdb::LightdbRequestHandler(lightdb);
    server->SetRequestHandler(requestHandler);
    lightdb::init_all();

    server->Listen(10000);
    server->Loop();
    delete server;
}
