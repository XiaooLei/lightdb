//
// Created by 9 on 2022/4/1.
//


#include "cJSON.h"
#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>


void TestRaftJson(){
    std::string raftConfig;
    int fd = open("raft.json", O_RDWR|O_CREAT);
    char buf[20];
    memset(buf, '\n', sizeof(buf));
    for(;;){
        int nread = read(fd, buf, sizeof(buf));
        if(nread == 0){
            break;
        }
        raftConfig.append(buf);
    }
    //printf(raftConfig.c_str());


    cJSON* raftJson = cJSON_Parse(raftConfig.c_str());
    cJSON* servers = cJSON_GetObjectItem(raftJson, "servers");
    int servernum = cJSON_GetArraySize(servers);
    printf("servers: %d \n", servernum);

    cJSON* server1 = cJSON_GetArrayItem(servers, 1);

    cJSON* host1 = cJSON_GetObjectItem(server1, "host");

    printf("%s \n", host1->valuestring);

    printf("index:%d \n", cJSON_GetObjectItem(server1, "index")->valueint);

    printf("me:%d \n", cJSON_GetObjectItem(raftJson, "me")->valueint);

}

void testJson(){
    char *string = "{\"family\":[\"father\",\"mother\",\"brother\",\"sister\",\"somebody\"]}";
    cJSON* cJson = cJSON_Parse(string);
    cJSON* node = cJSON_GetObjectItem(cJson,"family");
    if(node != NULL){
        int family_size = cJSON_GetArraySize(node);
        printf("family size = %d \n", family_size);
    }
}

int main(){
    TestRaftJson();
    return 0;
}

