//
// Created by 9 on 2022/3/15.
//

#ifndef MYPROJECT_CMD_HANDLER_H
#define MYPROJECT_CMD_HANDLER_H


#include "cmd.h"
namespace lightdb {

    class RequestHandler {

    private:
        LightDB* lightDb;

    public:

        virtual std::string HandleCmd(std::string request, int conn_fd) = 0;


    };

}

#endif //MYPROJECT_CMD_HANDLER_H
