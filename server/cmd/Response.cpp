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

    void Response::Decode(const std::string &bytes) {
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        this->respCode = cJSON_GetObjectItem(cJson, "RespCode")->valueint;
        this->respContent = cJSON_GetObjectItem(cJson, "RespContent")->valuestring;
    }


}
