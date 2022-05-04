//
// Created by xiaolei on 2022/3/15.
//

#ifndef MYPROJECT_LIGHTDBREQUESTHANDLER_H
#define MYPROJECT_LIGHTDBREQUESTHANDLER_H


#include "requestHandler.h"
#include "../../sync/SafeQueue.h"
#include <unistd.h>

#include <utility>

namespace lightdb {

    struct Task{
        CmdFunc* cmdFunc;
        LightDB* lightDb;
        std::vector<std::string> args;
        int conn_fd;

        Task():cmdFunc(nullptr),lightDb(nullptr),conn_fd(-1){};

        Task(CmdFunc* cmdFunc, LightDB* lightDb, std::vector<std::string>  args, int conn_fd): cmdFunc(cmdFunc), lightDb(lightDb), args(std::move(args)), conn_fd(conn_fd){}
    };

    struct RespTask{
        int conn_fd;
        std::string resp;

        RespTask():conn_fd(-1){}

        RespTask(int conn_fd, std::string  resp):conn_fd(conn_fd), resp(std::move(resp)){}

    };


    class LightdbRequestHandler : public RequestHandler{
    private:
        LightDB* lightDb;

        //taskQueue
        SafeQueue<Task> strTaskQueue;
        SafeQueue<Task> hashTaskQueue;
        SafeQueue<Task> listTaskQueue;
        SafeQueue<Task> setTaskQueue;
        SafeQueue<Task> zsetTaskQueue;

        //responseQueue
        SafeQueue<RespTask> respQueue;


    public:

        explicit LightdbRequestHandler(LightDB* db):lightDb(db){
            Start();
        }

        void Start();

        std::string HandleCmd(std::string request, int conn_fd) override;

    };


}


#endif //MYPROJECT_LIGHTDBREQUESTHANDLER_H
