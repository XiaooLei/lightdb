//
// Created by xiaolei on 2022/3/11.
//

#include "cmd.h"
#include "Response.h"

namespace lightdb{
void hSet(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() != 3){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    int count;
    s = db->HSet(args[0], args[1], args[2], count);
    if(!s.ok()){
        resp = Response::ResponseWrap(s.Code(), s.Message());
        return;
    }
    resp = to_string(count);
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hSetNx(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size()!=3){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    bool suc;
    s = db->HSetNx(args[0], args[1], args[2], suc);
    if(!s.ok()){
        resp = Response::ResponseWrap(s.Code(), s.Message());
        return;
    }
    resp = to_string(suc);
    resp = Response::ResponseWrap(s.Code(), resp);
}


void hGet(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size()!=2){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    bool suc = db->HGet(args[0], args[1],  resp);
    if(!suc){
        resp = "(nil)";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hGetAll(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    std::vector<std::string> vals;
    bool suc = db->HGetAll(args[0], vals);
    if(!suc){
        resp = "(empty set or list)";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    for(int i = 0; i < vals.size(); i++){
        resp.append(to_string(i + 1) + ") ");
        resp.append(vals[i] + "\n");
    }
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hMSet(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size()%2 != 1){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    std::string key = args[0];
    auto args1 = args;
    args1.erase(args1.begin());
    s = db->HMSet(key, args1);
    if(!s.ok()){
        resp = Response::ResponseWrap(s.Code(), s.Message());
        return;
    }
    resp = "OK";
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hMGet(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() <=1 ){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    std::string key = args[0];
    auto args1 = args;
    args1.erase(args1.begin());
    std::vector<std::string> vals;
    std::vector<bool> sucs;
    s = db->HMGet(key, args, vals, sucs);
    if(!s.ok()){
        resp = Response::ResponseWrap(s.Code(), s.Message());
        return;
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
}


//Hash Del
void hDel(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() <= 1){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    int suc;
    int count = 0;
    for(int i = 1; i <= args.size(); i++) {
        s = db->HDel(args[0], args[i], suc);
        if (!s.ok()) {
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        count += suc;
    }
    resp.append("(integer) ");
    resp.append(to_string(count));
    resp = Response::ResponseWrap(s.Code(), resp);
}


void hKeyExist(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    bool exist = db->HKeyExist(args[0]);
    if(exist){
        resp = "1";
    }else{
        resp = "0";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hExist(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() != 2){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    bool exist = db->HExist(args[0], args[1]);
    if(exist){
        resp = "1";
    }else{
        resp = "0";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hLen(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    int len = db->HLen(args[0]);
    resp.append("(integer) ");
    resp.append(to_string(len));
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hKeys(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    if(args.size() != 1){
        resp = "wrong num of args";
        return;
    }
    Status s;
    std::vector<std::string> keys;
    bool exist = db->HKeys(args[0], keys);
    if(exist){
        for(int i = 0; i<keys.size(); i++){
            if(i!=0){
                resp.append( "\n");
            }
            resp.append(to_string(i+1) + ")" + keys[i] );
        }
    }else{
        resp = "(empty list or set)";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hVals(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        return;
    }
    std::vector<std::string> vals;
    bool exist = db->HVals(args[0], vals);
    if(exist){
        for(int i = 0; i<vals.size(); i++){
            if(i!=0) {
                resp.append( "\n");
            }
            resp.append(to_string(i+1) + ")" + vals[i] );
        }
    }else{
        resp = "(empty list or set)";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hClear(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    int count = 0;
    s = db->HClear(args[0], count);
    if(!s.ok()){
        resp = Response::ResponseWrap(s.Code(), s.Message());
        return;
    }
    resp.append("(integer) ");
    resp.append(to_string(count));
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hExpire(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() != 2){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    int count = 0;
    bool suc;
    s = db->HExpire(args[0], strtoull(args[1].c_str(), nullptr, 10), suc);
    if(!s.ok()){
        resp = Response::ResponseWrap(s.Code(), s.Message());
        return;
    }
    if(suc){
        resp = "(integer) 1";
    }else{
        resp = "(integer) 0";
    }
    resp = Response::ResponseWrap(s.Code(), resp);
}

void hTTL(LightDB* db, const std::vector<std::string>& args, std::string& resp){
    Status s;
    if(args.size() != 1){
        resp = "wrong num of args";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }
    int64_t ttl = db->HTTL(args[0]);
    resp.append("(integer) ");
    resp.append(to_string(ttl));
    resp = Response::ResponseWrap(s.Code(), resp);
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
