//
// Created by xiaolei on 2022/3/12.
//

#include "cmd.h"
#include "Response.h"
namespace lightdb{
    void sAdd(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() <= 1) {
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int count = 0;
        for(int i = 1; i<args.size(); i++) {
            s = db->SAdd(args[0], args[i], count);
            if(!s.ok()){
                resp = Response::ResponseWrap(s.Code(), s.Message());
                return;
            }
        }
        resp.append("(integer) ");
        resp.append(to_string(count));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void sPop(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1) {
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        std::string value;
        bool suc;
        s = db->SPop(args[0], value, suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        if(suc){
            resp.append(value);
        }else{
            resp.append("(nil)");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
   }

    void sIsMember(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool isMember = db->SIsMember(args[0], args[1]);
        resp.append("(integer) ");
        resp.append(to_string(isMember));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void sRem(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() <= 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool suc;
        int count = 0;
        for(int i = 1; i < args.size(); i++){
            s = db->SRem(args[0], args[i], suc);
            if(!s.ok()){
                resp = Response::ResponseWrap(s.Code(), s.Message());
                return;
            }
            if(suc){
                count++;
            }
        }
        resp.append(to_string(count));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void sMove(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool suc;
        s = db->SMove(args[0], args[1], args[2], suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        if(suc){
            resp = "(integer) 1";
        }else{
            resp = "(integer 0)";
        }
        resp = Response::ResponseWrap(s.Code(), resp);
    }


    void sCard(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int card = db->SCard(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(card));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void sMemebers(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        std::vector<std::string> members;
        bool suc = db->SMembers(args[0], members);
        if(suc){
            for(int i = 0; i < members.size(); i++){
                resp.append(to_string(i+1) + ") ");
                resp.append(members[i] + "\n");
            }
        }else{
            resp = "(empty list or set)";
        }
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void sUnion(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.empty()){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        std::vector<std::string> vals;
        db->SUnion(args, vals);
        if(vals.empty()){
            resp = "(empty set or list)";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        for(int i = 0; i < vals.size(); i++){
            resp.append(to_string(i+1) + ") ");
            resp.append(vals[i] + "\n");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void sDiff(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() <= 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        std::vector<std::string> vals;
        std::string key1 = args[0];
        auto args1 = args;
        args1.erase(args1.begin());
        db->SDiff(key1, args1, vals);
        if(vals.empty()){
            resp = "(empty set or list)";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        for(int i = 0; i < vals.size(); i++){
            resp.append(to_string(i+1) + ") ");
            resp.append(vals[i] + "\n");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void sKeyExist(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool exist = db->SKeyExist(args[0]);
        resp = to_string(exist);
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void sClear(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int count = 0;
        s = db->SClear(args[0], count);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return;
        }
        resp.append("(integer) ");
        resp.append(to_string(count));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void sExpire(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        bool suc;
        s = db->SExpire(args[0], strtoull(args[1].c_str(), nullptr, 10), suc);
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

    void sTTL(LightDB* db, const std::vector<std::string>& args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return;
        }
        int64_t ttl = db->STTL(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(ttl));
        resp = Response::ResponseWrap(s.Code(), resp);
    }

    void init_set(){
        addExecCommand("sadd", sAdd);
        addExecCommand("spop", sPop);
        addExecCommand("sismember", sIsMember);
        addExecCommand("srem", sRem);
        addExecCommand("smove", sMove);
        addExecCommand("scard", sCard);
        addExecCommand("smembers", sMemebers);
        addExecCommand("sunion", sUnion);
        addExecCommand("sdiff", sDiff);
        addExecCommand("skeyexist", sKeyExist);
        addExecCommand("sclear", sClear);
        addExecCommand("sexpire", sExpire);
        addExecCommand("sttl", sTTL);
    }

}
