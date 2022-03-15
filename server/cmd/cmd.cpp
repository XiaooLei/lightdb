//
// Created by 9 on 2022/3/14.
//

#include "cmd.h"
namespace lightdb{
    std::map<std::string, CmdFunc*> ExecCmd;
    void addExecCommand(std::string cmd, CmdFunc cmdFunc){
        ExecCmd.insert(std::make_pair(cmd, cmdFunc));
    }
    CmdFunc* GetCmdFunction(std::string cmd) {
        if(ExecCmd.find(cmd) == ExecCmd.end()){
            return nullptr;
        }
        return ExecCmd[cmd];
    }

    void init_all(){
        init_hash();
        init_list();
        init_set();
        init_str();
        init_zset();
    }

}