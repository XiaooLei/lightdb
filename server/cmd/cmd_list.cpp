//
// Created by xiaolei on 2022/3/12.
//

#include "cmd.h"
#include "Response.h"
namespace lightdb{
    Status lPush(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() < 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int length = 0;
        for(int i = 1; i < args.size(); i++) {
            s = db->LPush(args[0], args[i], length);
            if (!s.ok()) {
                resp = Response::ResponseWrap(s.Code(), s.Message());
                return s;
            }
        }
        resp.append("(integer) ");
        resp.append(to_string(length));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status rPush(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() < 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int length = 0;
        for(int i = 1; i < args.size(); i++) {
            s = db->RPush(args[0], args[i], length);
            if (!s.ok()) {
                resp = Response::ResponseWrap(s.Code(), s.Message());
                return s;
            }
        }
        resp.append("(integer) ");
        resp.append(to_string(length));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status rPop(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size()!=1){
            resp = "wrong num of args";
            return Status::OK();
        }
        Status s;
        bool suc;
        s = db->RPop(args[0], resp, suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lPop(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size()!=1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool suc;
        s = db->LPop(args[0], resp, suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lIndex(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size()!=2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool suc;
        bool valid;
        uint32_t idx = stringToInt(args[1], valid);
        if(!valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        suc  = db->LIndex(args[0], idx, resp);
        if(!suc){
            resp = "(nil)";
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lRem(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() < 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool valid;
        int count = stringToInt(args[2], valid);
        if(!valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int removed;
        s = db->LRem(args[0], args[1], count, removed);
        resp.append("(integer) ");
        resp.append(to_string(removed));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lInsert(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 4){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }

        InsertOption option;
        if(args[1].compare("BEFORE") == 0){
            option = Before;
        }else if(args[1].compare("AFTER") == 0){
            option = After;
        }else{
            resp = "wrong args: option should be BEFORE|AFTER";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int count = 0;
        s = db->LInsert(args[0], option, args[2], args[3],count);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lSet(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool suc;
        bool valid;
        int index = stringToInt(args[1], valid);
        if(!valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        s = db->LSet(args[0], index, args[2], suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        if(suc){
            resp.append("(integer) 1");
        }else{
            resp.append("(integer) 0");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lTrim(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool start_valid;
        int start = stringToInt(args[1], start_valid);
        bool end_valid;
        int end = stringToInt(args[2], end_valid);
        if(!(start_valid && end_valid)){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool suc;
        s = db->LTrim(args[0], start, end, suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lRange(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
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
            return s;
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
        return s;
    }

    Status lLen(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 1) {
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int len = db->LLen(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(len));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lKeyExist(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool exist = db->LKeyExist(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(exist));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lClear(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int count = 0;
        s = db->LClear(args[0], count);
        if(!s.ok()){
            return s;
        }
        resp.append("(integer) ");
        resp.append(to_string(count));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status lExpire(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int count = 0;
        bool suc;
        s = db->LExpire(args[0], strtoull(args[1].c_str(), nullptr, 10), suc);
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

    Status lTTL(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        auto ttl = db->LTTL(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(ttl));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
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

