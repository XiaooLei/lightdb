//
// Created by 9 on 2022/3/10.
//

#include "server.h"
#include "cmd/LightdbRequestHandler.h"

int main(){

    lightdb::Server* server = new lightdb::Server();

    lightdb::LightDB* lightdb = new lightdb::LightDB();
    lightdb::Config config = lightdb::Config::BuildConfig("/tmp/lightdb/config.ini");
    lightdb->Open(&config);

    lightdb::RequestHandler* requestHandler = new lightdb::LightdbRequestHandler(lightdb);
    server->SetRequestHandler(requestHandler);
    lightdb::init_all();

    server->Listen(10000);
    server->Loop();
    delete server;
}
