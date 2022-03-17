//
// Created by 9 on 2022/3/14.
//
#include "../../lightdb.h"

namespace lightdb {
    typedef Status CmdFunc(LightDB* db, std::vector<std::string> args, std::string& res);

    void addExecCommand(std::string cmd, CmdFunc cmdFunc);

    CmdFunc* GetCmdFunction(std::string cmd);

    void init_hash();

    void init_list();

    void init_set();

    void init_str();

    void init_zset();

    void init_all();

}