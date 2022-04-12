//
// Created by xiaolei on 2022/3/11.
//

#include "cmd.h"
#include "Response.h"

namespace lightdb{
Status hSet(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() != 3){
        resp = "wrong num of args";
        return Status::OK();
    }
    int count;
    s = db->HSet(args[0], args[1], args[2], count);
    if(!s.ok()){
        return s;
    }
    resp = to_string(count);
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

Status hSetNx(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size()!=3){
        resp = "wrong num of args";
        return Status::OK();
    }
    bool suc;
    s = db->HSetNx(args[0], args[1], args[2], suc);
    if(!s.ok()){
        return s;
    }
    resp = to_string(suc);
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}


Status hGet(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size()!=2){
        resp = "wrong num of args";
        return Status::OK();
    }
    bool suc = db->HGet(args[0], args[1],  resp);
    if(!suc){
        resp = "(nil)";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

Status hGetAll(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        return Status::OK();
    }
    std::vector<std::string> vals;
    bool suc = db->HGetAll(args[0], vals);
    if(!suc){
        resp = "(empty set or list)";
        return Status::OK();
    }
    for(int i = 0; i < vals.size(); i++){
        resp.append(to_string(i + 1) + ") ");
        resp.append(vals[i] + "\n");
    }
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

Status hMSet(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size()%2 != 1){
        resp = "wrong num of args";
        return Status::OK();
    }
    std::string key = args[0];
    args.erase(args.begin());
    s = db->HMSet(key, args);
    if(!s.ok()){
        return s;
    }
    resp = "OK";
    resp = Response::ResponseWrap(s.Code(), resp);
    Status::OK();
}

Status hMGet(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() <=1 ){
        resp = "wrong num of args";
        return Status::OK();
    }
    std::string key = args[0];
    args.erase(args.begin());
    std::vector<std::string> vals;
    std::vector<bool> sucs;
    s = db->HMGet(key, args, vals, sucs);
    if(!s.ok()){
        return s;
    }
    for(int i = 0; i < vals.size(); i++){
        resp.append(to_string(i + 1));
        resp.append(") ");
        if(sucs[i]){
            resp.append(vals[i]);
        }else{
            resp.append("(nil)");
        }
        resp.append("\n");
    }
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}


//Hash Del
Status hDel(LightDB* db, std::vector<std::string> args, std::string& resp){
    if(args.size() <= 1){
        resp = "wrong num of args";
        return Status::OK();
    }
    Status s;
    int suc;
    int count = 0;
    for(int i = 1; i <= args.size(); i++) {
        s = db->HDel(args[0], args[i], suc);
        if (!s.ok()) {
            return s;
        }
        count += suc;
    }
    resp.append("(integer) ");
    resp.append(to_string(count));
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}


Status hKeyExist(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        return Status::OK();
    }
    bool exist = db->HKeyExist(args[0]);
    if(exist){
        resp = "1";
    }else{
        resp = "0";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

Status hExist(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() != 2){
        resp = "wrong num of args";
        return Status::OK();
    }
    bool exist = db->HExist(args[0], args[1]);
    if(exist){
        resp = "1";
    }else{
        resp = "0";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

Status hLen(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        return Status::OK();
    }
    int len = db->HLen(args[0]);
    resp.append("(integer) ");
    resp.append(to_string(len));
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

Status hKeys(LightDB* db, std::vector<std::string> args, std::string& resp){
    if(args.size() != 1){
        resp = "wrong num of args";
        return Status::OK();
    }
    Status s;
    std::vector<std::string> keys;
    bool exist = db->HKeys(args[0], keys);
    if(exist){
        for(int i = 0; i<keys.size(); i++){
            resp.append(to_string(i+1));
            resp.append(keys[i] + "\n");
        }
    }else{
        resp = "(empty list or set)";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

Status hVals(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        return Status::OK();
    }
    std::vector<std::string> vals;
    bool exist = db->HVals(args[0], vals);
    if(exist){
        for(int i = 0; i<vals.size(); i++){
            resp.append(to_string(i+1));
            resp.append(vals[i] + "\n");
        }
    }else{
        resp = "(empty list or set)";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

Status hClear(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }
    int count = 0;
    s = db->HClear(args[0], count);
    if(!s.ok()){
        resp = Response::ResponseWrap(s.Code(), s.Message());
        return s;
    }
    resp.append("(integer) ");
    resp.append(to_string(count));
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

Status hExpire(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() != 2){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }
    int count = 0;
    bool suc;
    s = db->HExpire(args[0], strtoull(args[1].c_str(), nullptr, 10), suc);
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

Status hTTL(LightDB* db, std::vector<std::string> args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }
    int64_t ttl = db->HTTL(args[0]);
    resp.append("(integer) ");
    resp.append(to_string(ttl));
    resp = Response::ResponseWrap(s.Code(), resp);
    return Status::OK();
}

void init_hash(){
    addExecCommand("hset", hSet);
    addExecCommand("hsetnx", hSetNx);
    addExecCommand("hget", hGet);
    addExecCommand("hgetall", hGetAll);
    addExecCommand("hmset", hMSet);
    addExecCommand("hmget", hMGet);
    addExecCommand("hdel", hDel);
    addExecCommand("hkeyexist", hKeyExist);
    addExecCommand("hexist", hExist);
    addExecCommand("hlen", hLen);
    addExecCommand("hkeys", hKeys);
    addExecCommand("hvals", hVals);
    addExecCommand("hclear", hClear);
    addExecCommand("hexpire", hExpire);
    addExecCommand("httl", hTTL);
}


}
