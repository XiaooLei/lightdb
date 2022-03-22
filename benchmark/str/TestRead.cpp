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

    uint64_t start_t = getCurrentTimeStamp();
    int times = 500000;
    std::vector<std::string> vals;
    for(int i = 0; i < times; i++){
        std::string key = "key-" + to_string(rand()%times);
        std::string value;
        bool suc;
        Status s;
        s = lightdb->Get(key, value, suc);
        if(s.ok()){
           vals.push_back(value);
        }
    }
    uint64_t end_t = getCurrentTimeStamp();
    printf("前20个value\n");
    for(int i = 0; i < 20; i++){
        printf("%s \n", vals[i].c_str());
    }
    double elapsed = end_t - start_t;
    printf("time_spend:%f microseconds \n", elapsed);
    double seconds = elapsed / 1000;
    double qps = times / seconds;
    printf("qps :% f read (random reads)/s \n", qps);

}

