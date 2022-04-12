//
// Created by 9 on 2022/4/12.
//

#ifndef MYPROJECT_RESPONSE_H
#define MYPROJECT_RESPONSE_H

#include <string>
#include "../../util/cJson/cJSON.h"

namespace lightdb {

class Response {
private:
    int respCode;
    std::string respContent;

public:
    Response(){}

    Response(const int &respCode, const std::string &respContent) : respCode(respCode), respContent(respContent) {}

    void Encode(std::string &bytes);

    void Decode(const std::string &bytes);

    static std::string ResponseWrap(int RespCode, const std::string& respContent) {
        Response response(RespCode, respContent);
        std::string respStr;
        response.Encode(respStr);
        return respStr;
    }

    std::string GetContent(){
        return respContent;
    }

    int GetRespCode(){
        return respCode;
    }

};


}


#endif //MYPROJECT_RESPONSE_H
