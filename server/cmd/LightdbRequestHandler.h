//
// Created by 9 on 2022/3/15.
//

#ifndef MYPROJECT_LIGHTDBREQUESTHANDLER_H
#define MYPROJECT_LIGHTDBREQUESTHANDLER_H


#include "requestHandler.h"
namespace lightdb {

    class LightdbRequestHandler : public RequestHandler{
    private:
        LightDB* lightDb;

    public:

        LightdbRequestHandler(LightDB* db):lightDb(db){}

        std::string HandleCmd(std::string request);

    };


}


#endif //MYPROJECT_LIGHTDBREQUESTHANDLER_H
