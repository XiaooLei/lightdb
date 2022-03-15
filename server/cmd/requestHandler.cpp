//
// Created by 9 on 2022/3/15.
//


#include "requestHandler.h"
namespace lightdb{

    std::string RequestHandler::HandleCmd(std::string request) {
        std::vector<std::string> cmdAndArgs;
        splitStr(request, cmdAndArgs);
        std::string cmd = cmdAndArgs[0];
        CmdFunc* handler = GetCmdFunction(cmd);
        cmdAndArgs.erase(cmdAndArgs.begin());
        std::string resp;

        Status s = handler(this->lightDb, cmdAndArgs, resp);
        if(!s.ok()){
            resp = "Internal Error: ";
            resp.append(s.Code());
        }
        return resp;
    }

}