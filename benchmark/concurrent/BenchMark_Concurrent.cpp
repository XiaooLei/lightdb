//
// Created by xiaolei on 2022/3/23.
//


#include "../../client/lightdb_client.h"
#include "../../util/time.h"
#include <vector>
#include <thread>

using namespace lightdb;
int main(int argc, char *argv[]){
    if(argc != 5){
        printf("wrong args, only %d args provided \n", argc);
        exit(0);
    }
    std::string address;
    address.assign(argv[1]);

    int port = atoi(argv[2]);

    int ConcurrentNum = atoi(argv[3]);
    int timesPerClient = atoi(argv[4]);

    std::vector<LightdbClient> clients;

    for(int i = 0; i < ConcurrentNum; i++){
        clients.push_back(LightdbClient());
    }
    for(int i = 0; i < ConcurrentNum; i++){
        clients[i].Connect(address, port);
    }

    uint64_t start_t = getCurrentTimeStamp();

    std::vector<std::string> resps;
    for(int i = 0; i < ConcurrentNum; i++){
        std::thread clientThread([&](){
            LightdbClient* client = &clients[i];
            for(int j = 0; j < timesPerClient; j++) {
                std::string resp;
                if (client->Execute("get keya", resp) < 0) {
                    printf("execute failed");
                    continue;
                }
                resps.push_back(resp);
            }
        });
        clientThread.join();
    }

    clients;

    printf("resps size: %d \n", resps.size());
    for(int i = 0; i < 10; i++){
        printf("resp: %s \n", resps[i].c_str());
    }

    uint64_t end_t = lightdb::getCurrentTimeStamp();
    double elapsed = end_t - start_t;
    printf(" %d times write，time_spend:%lu microseconds \n", ConcurrentNum * timesPerClient, elapsed);
    double seconds = elapsed / 1000;
    double qps = (double )(ConcurrentNum * timesPerClient) / seconds;
    printf("qps :% f read/s \n", qps);

    return 0;
}

