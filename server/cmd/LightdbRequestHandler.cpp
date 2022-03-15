//
// Created by 9 on 2022/3/15.
//

#include "LightdbRequestHandler.h"


namespace lightdb{


    std::string LightdbRequestHandler::HandleCmd(std::string request) {
        printf("request:%s  \n", request.c_str());
        std::vector<std::string> cmdAndArgs;
        splitStrBySpace(request, cmdAndArgs);
        if(cmdAndArgs.size() == 0){
            return "";
        }
        printf("size: %d \n", cmdAndArgs.size());
        std::string cmd = cmdAndArgs[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        CmdFunc* handler = GetCmdFunction(cmd);
        if(handler == nullptr){
            return "no such command";
        }
        cmdAndArgs.erase(cmdAndArgs.begin());
        std::string resp;

        printf("lightdb null :%d \n", this->lightDb == nullptr);

        Status s = handler(this->lightDb, cmdAndArgs, resp);
        if(!s.ok()){
            resp = "Internal Error: ";
            resp.append(to_string(s.Code()));
        }
        printf("HandleCmd resp:%s \n", resp.c_str());
        return resp;
    }


}