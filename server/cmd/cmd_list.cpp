//
// Created by xiaolei on 2022/3/12.
//

#include "cmd.h"
#include "Response.h"
namespace lightdb{
    void lPush(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() < 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int length = 0;
        for(int i = 1; i < args.size(); i++) {
            s = db->LPush(args[0], args[i], length);
            if (!s.ok()) {
                resp = Response::ResponseWrap(s.Code(), s.Message());
                return;
            }
        }
        resp.append("(integer) ");
        resp.append(to_string(length));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void rPush(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() < 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int length = 0;
        for(int i = 1; i < args.size(); i++) {
            s = db->RPush(args[0], args[i], length);
            if (!s.ok()) {
                resp = Response::ResponseWrap(s.Code(), s.Message());
                return;
            }
        }
        resp.append("(integer) ");
        resp.append(to_string(length));
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }

    void rPop(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size()!=1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool suc;
        s = db->RPop(args[0], resp, suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }

    void lPop(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size()!=1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool suc;
        s = db->LPop(args[0], resp, suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }

    void lIndex(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size()!=2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool suc;
        bool valid;
        uint32_t idx = stringToInt(args[1], valid);
        if(!valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        suc  = db->LIndex(args[0], idx, resp);
        if(!suc){
            resp = "(nil)";
        }
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void lRem(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() < 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool valid;
        int count = stringToInt(args[2], valid);
        if(!valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int removed;
        s = db->LRem(args[0], args[1], count, removed);
        resp.append("(integer) ");
        resp.append(to_string(removed));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void lInsert(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 4){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }

        InsertOption option;
        if(args[1].compare("BEFORE") == 0){
            option = Before;
        }else if(args[1].compare("AFTER") == 0){
            option = After;
        }else{
            resp = "wrong args: option should be BEFORE|AFTER";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int count = 0;
        s = db->LInsert(args[0], option, args[2], args[3],count);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        resp = "OK";
        resp = Response::ResponseWrap(s.Code(), resp);
        return;
    }

    void lSet(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool suc;
        bool valid;
        int index = stringToInt(args[1], valid);
        if(!valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        s = db->LSet(args[0], index, args[2], suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        if(suc){
            resp.append("(integer) 1");
        }else{
            resp.append("(integer) 0");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void lTrim(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool start_valid;
        int start = stringToInt(args[1], start_valid);
        bool end_valid;
        int end = stringToInt(args[2], end_valid);
        if(!(start_valid && end_valid)){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool suc;
        s = db->LTrim(args[0], start, end, suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        resp = "OK";
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void lRange(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        std::vector<std::string> vals;
        bool start_valid;
        int start = stringToInt(args[1], start_valid);
        bool end_valid;
        int end = stringToInt(args[2], end_valid);
        bool suc;
        s = db->LRange(args[0], start, end, vals, suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        if(vals.size() == 0){
            resp = "(empty set or list)";
        }
        for(int i = 0; i < vals.size(); i++){
            resp.append(to_string(i + 1) );
            resp.append(") ");
            resp.append(vals[i]);
            resp.append("\n");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void lLen(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1) {
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int len = db->LLen(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(len));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void lKeyExist(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool exist = db->LKeyExist(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(exist));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void lClear(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int count = 0;
        s = db->LClear(args[0], count);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        resp.append("(integer) ");
        resp.append(to_string(count));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void lExpire(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int count = 0;
        bool suc;
        s = db->LExpire(args[0], strtoull(args[1].c_str(), nullptr, 10), suc);
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

    void lTTL(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        auto ttl = db->LTTL(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(ttl));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void init_list(){
        addExecCommand("lpush", lPush);
        addExecCommand("rpush", rPush);
        addExecCommand("lpop", lPop);
        addExecCommand("rpop", rPop);
        addExecCommand("lindex", lIndex);
        addExecCommand("lrem", lRem);
        addExecCommand("linsert", lInsert);
        addExecCommand("lset", lSet);
        addExecCommand("ltrim", lTrim);
        addExecCommand("lrange", lRange);
        addExecCommand("llen", lLen);
        addExecCommand("lkeyexist", lKeyExist);
        addExecCommand("lclear", lClear);
        addExecCommand("lexpire", lExpire);
        addExecCommand("lttl", lTTL);
    }

}

