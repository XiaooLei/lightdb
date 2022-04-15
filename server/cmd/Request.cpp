//
// Created by xiaolei on 2022/4/14.
//

#include "Request.h"
namespace lightdb{
    Request::Request(const int &reqCode, const std::string &requestContent): reqCode(reqCode), reqContent(requestContent){}

    void Request::Encode(std::string &bytes) {
        cJSON* cJson = cJSON_CreateObject();
        cJSON_AddStringToObject(cJson, "ReqContent", reqContent.c_str());
        cJSON_AddNumberToObject(cJson, "ReqCode", reqCode);
        bytes = cJSON_Print(cJson);
    }

    void Request::Decode(std::string bytes) {
        cJSON* cJson = cJSON_Parse(bytes.c_str());
        this->reqCode = cJSON_GetObjectItem(cJson, "ReqCode")->valueint;
        this->reqContent = cJSON_GetObjectItem(cJson, "ReqContent")->valuestring;
    }

    RequestType Request::GetReqType() {
        return (RequestType)this->reqCode;
    }

    std::string Request::GetReqContent() {
        return this->reqContent;
    }

}