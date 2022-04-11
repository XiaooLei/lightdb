//
// Created by xiaolei on 2022/3/22.
//
#include "../../lightdb.h"
using namespace lightdb;

int main(){
    LightDB* lightdb = new lightdb::LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    int times = 1 00000;
    for(int i = 0; i < times; i++){
        std::string key = "listkey";
        std::string value = "listvalue-" + to_string(i);
        int length;
        Status s;
        s = lightdb->LPush(key, value, length);
        if(s.ok()){
            printf("");
        }
    }



    return 0;
}


