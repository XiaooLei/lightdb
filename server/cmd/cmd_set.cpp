//
// Created by 9 on 2022/3/12.
//

#include "cmd.h"
namespace lightdb{
    Status sAdd(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() <= 1) {
            resp = "wrong num of args";
            return Status::OK();
        }

        Status s;
        int count = 0;
        for(int i = 1; i<args.size(); i++) {
            s = db->SAdd(args[0], args[i], count);
            if(!s.ok()){
                return s;
            }
        }
        resp.append("(integer) ");
        resp.append(to_string(count));
        return Status::OK();
    }

    Status sPop(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() != 1) {
            resp = "wrong num of args";
            return Status::OK();
        }
        Status s;
        std::string value;
        bool suc;
        s = db->SPop(args[0], value, suc);
        if(!s.ok()){
            return s;
        }
        if(suc){
            resp.append(value);
        }else{
            resp.append("(nil)");
        }
        return Status::OK();
    }

    Status sIsMember(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() != 2){
            resp = "wrong num of args";
            return Status::OK();
        }
        Status s;
        bool isMember = db->SIsMember(args[0], args[1]);
        resp.append("(integer) ");
        resp.append(to_string(isMember));
        return Status::OK();
    }

    Status sRem(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() <= 1){
            resp = "wrong num of args";
            return Status::OK();
        }
        Status s;
        bool suc;
        int count = 0;
        for(int i = 1; i < args.size(); i++){
            s = db->SRem(args[0], args[i], suc);
            if(!s.ok()){
                return s;
            }
            if(suc){
                count++;
            }
        }
        resp.append(to_string(count));
        return Status::OK();
    }

    Status sMove(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() != 3){
            resp = "wrong num of args";
            return Status::OK();
        }
        Status s;
        bool suc;
        s = db->SMove(args[0], args[1], args[2], suc);
        if(!s.ok()){
            return s;
        }
        if(suc){
            resp = "(integer) 1";
        }else{
            resp = "(integer 0)";
        }
        return Status::OK();
    }


    Status sCard(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() != 1){
            resp = "wrong num of args";
            return Status::OK();
        }
        int card = db->SCard(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(card));
        return Status::OK();
    }

    Status sMemebers(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() != 1){
            resp = "wrong num of args";
            return Status::OK();
        }
        Status s;
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
        return Status::OK();
    }

    Status sUnion(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() <= 0){
            resp = "wrong num of args";
            return Status::OK();
        }
        std::vector<std::string> vals;
        db->SUnion(args, vals);
        if(vals.size() == 0){
            resp = "(empty set or list)";
            return Status::OK();
        }
        for(int i = 0; i < vals.size(); i++){
            resp.append(to_string(i+1) + ") ");
            resp.append(vals[i] + "\n");
        }

        return Status::OK();
    }

    Status sDiff(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() <= 1){
            resp = "wrong num of args";
            return Status::OK();
        }
        std::vector<std::string> vals;
        std::string key1 = args[0];
        args.erase(args.begin());
        db->SDiff(key1, args, vals);
        if(vals.size() == 0){
            resp = "(empty set or list)";
            return Status::OK();
        }
        for(int i = 0; i < vals.size(); i++){
            resp.append(to_string(i+1) + ") ");
            resp.append(vals[i] + "\n");
        }
        return Status::OK();
    }


    Status sKeyExist(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() != 1){
            resp = "wrong num of args";
            return Status::OK();
        }
        bool exist = db->SKeyExist(args[0]);
        resp = to_string(exist);
        return Status::OK();
    }

    Status sClear(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() != 1){
            resp = "wrong num of args";
            return Status::OK();
        }
        Status s;
        int count = 0;
        s = db->SClear(args[0], count);
        if(!s.ok()){
            return s;
        }
        resp.append("(integer) ");
        resp.append(to_string(count));
        return Status::OK();
    }

    Status sExpire(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() != 2){
            resp = "wrong num of args";
            return Status::OK();
        }
        Status s;
        int count = 0;
        bool suc;
        s = db->SExpire(args[0], strtoull(args[1].c_str(), nullptr, 10), suc);
        if(!s.ok()){
            return s;
        }
        if(suc){
            resp = "(integer) 1";
        }else{
            resp = "(integer) 0";
        }
        return Status::OK();
    }

    Status sTTL(LightDB* db, std::vector<std::string> args, std::string& resp){
        if(args.size() != 1){
            resp = "wrong num of args";
            return Status::OK();
        }
        Status s;
        int64_t ttl = db->STTL(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(ttl));
        return Status::OK();
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
