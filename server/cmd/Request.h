//
// Created by xiaolei on 2022/4/14.
//

#ifndef MYPROJECT_REQUEST_H
#define MYPROJECT_REQUEST_H

#include <string>
#include "../../util/cJson/cJSON.h"
namespace lightdb {

enum RequestType{ReadReq, WriteReq, VoteReq, AppendEntriesReq};

class Request {
    int reqCode;
    std::string reqContent;

public:
    Request():reqCode(-1){};

    Request(const int& reqCode, const std::string& requestContent);

    void Encode(std::string& bytes);

    void Decode(std::string bytes);

    std::string GetReqContent();

    RequestType GetReqType();

};


}

#endif //MYPROJECT_REQUEST_H
