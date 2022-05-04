//
// Created by xiaolei on 2022/4/12.
//

#ifndef MYPROJECT_RESPONSE_H
#define MYPROJECT_RESPONSE_H

#include <string>
#include <utility>
#include "../../util/cJson/cJSON.h"

namespace lightdb {

class Response {
private:
    int respCode;
    std::string respContent;

public:
    Response():respCode(-1){}

    Response(const int &respCode, std::string respContent) : respCode(respCode), respContent(std::move(respContent)) {}

    void Encode(std::string &bytes);

    int Decode(const std::string &bytes);

    static std::string ResponseWrap(int RespCode, const std::string& respContent) {
        Response response(RespCode, respContent);
        std::string respStr;
        response.Encode(respStr);
        return respStr;
    }

    std::string GetContent(){
        return respContent;
    }

    int GetRespCode() const{
        return respCode;
    }

};


}


#endif //MYPROJECT_RESPONSE_H
