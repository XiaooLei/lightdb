//
// Created by xiaolei on 2022/3/14.
//

#include "cmd.h"
#include "Response.h"
namespace lightdb {
    Status set(LightDB *db, std::vector<std::string> args, std::string &resp) {
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        s = db->StrSet(args[0], args[1]);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp.append("OK");
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status get(LightDB *db, std::vector<std::string> args, std::string &resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        std::string value;
        bool suc;
        s = db->Get(args[0], value, suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        if(suc) {
            resp.append(value);
        }else{
            resp = "(nil)";
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status setNx(LightDB *db, std::vector<std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool suc;
        s = db->StrSetNx(args[0], args[1], suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp.append("(integer) ");
        resp.append(to_string(suc));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status getSet(LightDB *db, std::vector<std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        std::string oldValue;
        bool suc;
        s = db->GetSet(args[0], oldValue, args[1], suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        if(suc){
            resp.append(oldValue);
        }else{
            resp.append("(nil)");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status mSet(LightDB *db, std::vector<std::string> args, std::string &resp){
        Status s;
        if(args.size()%2 == 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        s = db->MSet(args);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp = "OK";
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status mGet(LightDB *db, std::vector<std::string> args, std::string &resp){
        Status s;
        if(args.size() == 0){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        std::vector<std::string> values;
        std::vector<bool> sucs;
        s = db->MGet(args, values, sucs);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        for(int i = 0; i < sucs.size(); i++){
            resp.append(to_string(i + 1));
            if(sucs[i]) {
                resp.append(") " + values[i]);
            }else{
                resp.append("(nil)");
            }
            resp.append("\n");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status appendStr(LightDB *db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int length = 0;
        s = db->Append(args[0], args[1], length);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp.append(to_string(length));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status strExist(LightDB *db, std::vector<std::string> args, std::string &resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool exist = db->StrExist(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(exist));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status remove(LightDB *db, std::vector<std::string> args, std::string &resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool suc;
        s = db->Remove(args[0], suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp.append("(integer) ");
        resp.append(to_string(suc));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status expire(LightDB *db, std::vector<std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int count = 0;
        bool suc;
        s = db->Expire(args[0], strtoull(args[1].c_str(), nullptr, 10), suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        if(suc){
            resp = "(integer) 1";
        }else{
            resp = "(integer) 0";
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status TTL(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int64_t ttl = db->TTL(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(ttl));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    void init_str(){
        addExecCommand("set", set);
        addExecCommand("get", get);
        addExecCommand("setnx", setNx);
        addExecCommand("getset", getSet);
        addExecCommand("mset", mSet);
        addExecCommand("mget", mGet);
        addExecCommand("appendstr", appendStr);
        addExecCommand("strexist", strExist);
        addExecCommand("remove", remove);
        addExecCommand("expire", expire);
        addExecCommand("ttl", TTL);
    }

}
