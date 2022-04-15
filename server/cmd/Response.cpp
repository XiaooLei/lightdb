//
// Created by 9 on 2022/4/12.
//

#include "Response.h"

namespace lightdb{

    void Response::Encode(std::string &bytes) {
        cJSON* cJson = cJSON_CreateObject();
        cJSON_AddStringToObject(cJson, "RespContent", respContent.c_str());
        cJSON_AddNumberToObject(cJson, "RespCode", respCode);
        bytes = cJSON_Print(cJson);
    }

    int Response::Decode(const std::string &bytes) {
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        cJSON* respCodeJson = cJSON_GetObjectItem(cJson, "RespCode");
        if(respCodeJson == nullptr){
            return -1;
        }
        this->respCode = respCodeJson->valueint;
        cJSON* respContentJson = cJSON_GetObjectItem(cJson, "RespContent");
        if(respContentJson == nullptr){
            return -1;
        }
        this->respContent = respContentJson->valuestring;
        return 0;
    }


}
