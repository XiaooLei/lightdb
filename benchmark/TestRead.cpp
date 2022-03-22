//
// Created by 9 on 2022/3/22.
//

#include "../lightdb.h"

using namespace lightdb;
int main(){
    LightDB* lightdb = new lightdb::LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);

    uint64_t start_t = getCurrentTimeStamp();
    double times = 10000;
    for(int i = 0; i < times; i++){
        std::string key = "key-" + to_string(i);
        std::string value;
        bool suc;
        Status s;
        s = lightdb->Get(key, value, suc);
        if(s.ok()){
           printf("value: %s \n", value.c_str());
        }
    }
    uint64_t end_t = getCurrentTimeStamp();
    double elapsed = end_t - start_t;
    printf("time_spend:%lu microseconds \n", elapsed);
    double seconds = elapsed / 1000;
    double qps = times / seconds;
    printf("qps :% f read (random reads)/s \n", qps);



}

