//
// Created by 9 on 2022/3/23.
//


#include "../../client/lightdb_client.h"
#include <vector>
using namespace lightdb;
int main(){

    int ConcurrentNum = 100;
    std::vector<LightdbClient> clients;
    for(int i = 0; i < ConcurrentNum; i++){
        clients.push_back(LightdbClient());
    }

    return 0;
}

