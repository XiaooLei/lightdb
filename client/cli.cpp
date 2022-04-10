//
// Created by 9 on 2022/3/23.
//

#define MAX_REQUEST_LENGTH 1000
#include "lightdb_client.h"
#include <iostream>
using namespace lightdb;
int main(){
    LightdbClient lightdbClient;
    if(lightdbClient.Connect("127.0.0.1") < 0){
        printf("connect failed");
        exit(0);
    }
    printf("successfully connected to lighted, address:%s \n", lightdbClient.RemoteAddress());
    while(true){
        printf("(%s:10000)>", lightdbClient.RemoteAddress());
        std::string resp;

        std::string request;
        std::getline(std::cin, request);
        if(request.size() == 0){
            continue;
        }
        //printf("request:%s||", request.c_str());
        if(request.compare("quit") == 0){
            lightdbClient.Close();
            exit(0);
        }
        if(lightdbClient.Execute(request, resp) < 0){
            printf("err occur\n");
            exit(0);
        }
        printf("%s\n", resp.c_str());
    }

}

