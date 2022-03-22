//
// Created by 9 on 2022/3/22.
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
    int keys = 2500;
    int values_per_set = 400;
    for(int i = 0; i < keys; i++){
        std::string key = "setkey-" + to_string(i);
        for(int i = 0; i < values_per_set; i++) {
            std::string value = "setvalue-" + to_string(i);
            int length;
            Status s;
            s = lightdb->SAdd(key, value, length);
        }
    }
    uint64_t end_t = lightdb::getCurrentTimeStamp();
    for(int i = 0; i < keys; i++){
        std::string key = "listkey-" + to_string(i);
        Status s;
        int count = 0;
        s = lightdb->SClear(key, count);
    }
    double elapsed = end_t - start_t;
    printf(" %d times write，time_spend:%lu microseconds \n", keys * values_per_set, elapsed);
    double seconds = elapsed / 1000;
    double qps = keys * values_per_set / seconds;
    printf("qps :% f write/s \n", qps);
    return 0;

}
