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

    //1kw 次写
    uint64_t start_t = getCurrentTimeStamp();
    int keys = 1000;
    int values_per_list = 1000;
    for(int i = 0; i < keys; i++){
        std::string key = "listkey-" + to_string(i);
        for(int i = 0; i < values_per_list; i++) {
            std::string value = "listvalue-" + to_string(i);
            int length;
            Status s;
            s = lightdb->LPush(key, value, length);
        }
    }
    uint64_t end_t = lightdb::getCurrentTimeStamp();
    for(int i = 0; i < keys; i++){
        std::string key = "listkey-" + to_string(i);
        Status s;
        int count = 0;
        s = lightdb->LClear(key, count);
    }
    double elapsed = end_t - start_t;
    printf(" %d times write，time_spend:%lu microseconds \n", keys * values_per_list, elapsed);
    double seconds = elapsed / 1000;
    double qps = keys * values_per_list / seconds;
    printf("qps :% f write/s \n", qps);

    return 0;
}


