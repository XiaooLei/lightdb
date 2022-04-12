//
// Created by xiaolei on 2022/3/23.
//
#include "../../client/lightdb_client.h"
#include "../../util/time.h"
#include <vector>
using namespace lightdb;
int main(){
    LightdbClient lightdbClient;
    lightdbClient.Connect("127.0.0.1", 10000);
    double times = 100000;

    uint64_t start_t = getCurrentTimeStamp();
    Response resp;
    std::vector<std::string> resps;
    for(int i = 0; i < times; i++){
        lightdbClient.Execute("test", resp);
        resps.push_back(resp.GetContent());
    }
    lightdbClient.Close();
    uint64_t end_t = lightdb::getCurrentTimeStamp();
    double elapsed = end_t - start_t;
    printf(" %d times write，time_spend:%lu microseconds \n", times, elapsed);
    double seconds = elapsed / 1000;
    double qps = times / seconds;
    printf("qps :% f read/s \n", qps);

    for(int i = 0; i < 20; i++){
        printf("%s \n", resps[i].c_str());
    }
    return 0;
}