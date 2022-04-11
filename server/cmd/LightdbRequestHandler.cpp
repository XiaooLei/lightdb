//
// Created by xiaolei on 2022/3/15.
//

#include "LightdbRequestHandler.h"


namespace lightdb{
    std::map<std::string, DataType> cmdDataTypeMap = {
            {"hset", Set},
            {"hsetnx",Set},
            {"hget", Set},
            {"hgetall", Set},
            {"hmset", Set},
            {"hmget", Set},
            {"hdel", Set},
            {"hkeyexist", Set},
            {"hexist", Set},
            {"hlen", Set},
            {"hkeys", Set},
            {"hvals", Set},
            {"hclear", Set},
            {"hexpire", Set},
            {"httl", Set},

            {"lpush", List},
            {"rpush", List},
            {"lpop", List},
            {"rpop", List},
            {"lindex", List},
            {"lrem", List},
            {"linsert", List},
            {"lset", List},
            {"ltrim", List},
            {"lrange", List},
            {"llen", List},
            {"lkeyexist", List},
            {"lclear", List},
            {"lexpire", List},
            {"lttl", List},

            {"sadd", Set},
            {"spop", Set},
            {"sismember", Set},
            {"srem", Set},
            {"smove", Set},
            {"scard", Set},
            {"smembers", Set},
            {"sunion", Set},
            {"sdiff", Set},
            {"skeyexist", Set},
            {"sclear", Set},
            {"sexpire", Set},
            {"sttl", Set},

            {"set", String},
            {"get", String},
            {"setnx", String},
            {"getset", String},
            {"mset", String},
            {"mget", String},
            {"appendstr", String},
            {"strexist", String},
            {"remove", String},
            {"expire", String},
            {"ttl", String},

            {"zadd", ZSet},
            {"zscore", ZSet},
            {"zcard", ZSet},
            {"zrank", ZSet},
            {"zrevrank", ZSet},
            {"zincyby", ZSet},
            {"zrange", ZSet},
            {"zrevrange", ZSet},
            {"zrem", ZSet},
            {"zgetbyrank", ZSet},
            {"zrevgetbyrank", ZSet},
            {"zkeyexist", ZSet},
            {"zclear", ZSet},
            {"zexpire", ZSet},
            {"zttl", ZSet},
    };

    void LightdbRequestHandler::Start() {

        //hash
        std::thread serveHash([&](){
            while(true) {
                Task task;
                bool get = this->hashTaskQueue.Dequeue(task);
                std::string resp;
                CmdFunc *handler = task.cmdFunc;
                Status s = handler(task.lightDb, task.args, resp);
                if (!s.ok()) {
                    resp = "Internal Error: ";
                    resp.append(to_string(s.Code()));
                }
                RespTask respTask(task.conn_fd, resp);
                this->respQueue.Enqueue(respTask);
            }
        });
        serveHash.detach();

        //set
        std::thread serveSet([&](){
            while(true) {
                Task task;
                bool get = this->setTaskQueue.Dequeue(task);
                std::string resp;
                CmdFunc *handler = task.cmdFunc;
                Status s = handler(task.lightDb, task.args, resp);
                if (!s.ok()) {
                    resp = "Internal Error: ";
                    resp.append(to_string(s.Code()));
                }
                RespTask respTask(task.conn_fd, resp);
                this->respQueue.Enqueue(respTask);
            }
        });
        serveSet.detach();

        //list
        std::thread serveList([&](){
            while(true) {
                Task task;
                bool get = this->listTaskQueue.Dequeue(task);
                std::string resp;
                CmdFunc *handler = task.cmdFunc;
                Status s = handler(task.lightDb, task.args, resp);
                if (!s.ok()) {
                    resp = "Internal Error: ";
                    resp.append(to_string(s.Code()));
                }
                RespTask respTask(task.conn_fd, resp);
                this->respQueue.Enqueue(respTask);
            }
        });
        serveList.detach();

        //str
        std::thread serveStr([&](){
            while(true) {
                Task task;
                bool get = this->strTaskQueue.Dequeue(task);
                std::string resp;
                CmdFunc *handler = task.cmdFunc;
                Status s = handler(task.lightDb, task.args, resp);
                if (!s.ok()) {
                    resp = "Internal Error: ";
                    resp.append(to_string(s.Code()));
                }
                RespTask respTask(task.conn_fd, resp);
                this->respQueue.Enqueue(respTask);
            }
        });
        serveStr.detach();

        //zset
        std::thread serveZSet([&](){
            while(true) {
                Task task;
                bool get = this->zsetTaskQueue.Dequeue(task);
                std::string resp;
                CmdFunc *handler = task.cmdFunc;
                Status s = handler(task.lightDb, task.args, resp);
                if (!s.ok()) {
                    resp = "Internal Error: ";
                    resp.append(to_string(s.Code()));
                }
                RespTask respTask(task.conn_fd, resp);
                this->respQueue.Enqueue(respTask);
            }
        });
        serveZSet.detach();

        //resp
        std::thread respThread([&](){
            RespTask respTask;
            while(true) {
                respQueue.Dequeue(respTask);
                //printf("consume respTask, conn_fd:%d, resp:%s \n", respTask.conn_fd, respTask.resp.c_str());
                write(respTask.conn_fd, respTask.resp.c_str(), respTask.resp.size());
            }
        });
        respThread.detach();
    }


    std::string LightdbRequestHandler::HandleCmd(std::string request, int conn_fd) {
        std::vector<std::string> cmdAndArgs;
        splitStrBySpace(request, cmdAndArgs);
        if(cmdAndArgs.size() == 0){
            return "";
        }
        std::string cmd = cmdAndArgs[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        CmdFunc* handler = GetCmdFunction(cmd);
        if(handler == nullptr){
            return "no such command";
        }
        cmdAndArgs.erase(cmdAndArgs.begin());
        std::string resp;

        //这里整一个队列
        DataType type = cmdDataTypeMap[cmd];
        switch (type) {
            case Hash:{
                Task task(handler, this->lightDb, cmdAndArgs, conn_fd);
                hashTaskQueue.Enqueue(task);
                break;
            }
            case List:{
                Task task(handler, this->lightDb, cmdAndArgs, conn_fd);
                listTaskQueue.Enqueue(task);
                break;
            }
            case Set:{
                Task task(handler, this->lightDb, cmdAndArgs, conn_fd);
                setTaskQueue.Enqueue(task);
                break;
            }
            case String:{
                Task task(handler, this->lightDb, cmdAndArgs, conn_fd);
                strTaskQueue.Enqueue(task);
                break;
            }
            case ZSet:{
                Task task(handler, this->lightDb, cmdAndArgs, conn_fd);
                zsetTaskQueue.Enqueue(task);
                break;
            }
            default:
                break;
        }

        return "";
    }


}
