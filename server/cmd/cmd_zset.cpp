//
// Created by xiaolei on 2022/3/14.
//
#include "cmd.h"
#include "Response.h"

namespace lightdb {
    Status zAdd(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool score_valid;
        double score = stringToDouble(args[1], score_valid);
        if(!score_valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool suc;
        s = db->ZAdd(args[0], score, args[2], suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp.append("(integer) ");
        resp.append(to_string(suc));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zScore(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        double score;
        bool suc = db->ZScore(args[0], args[1], score);
        if(suc){
            resp = to_string(score);
        }else{
            resp = "(nil)";
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zCard(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int card = db->ZCard(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(card));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zRank(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int rank = db->ZRank(args[0], args[1]);
        if(rank != -1) {
            resp.append(to_string(rank));
        }else{
            resp.append("(nil)");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zRevRank(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int rank = db->ZRevRank(args[0], args[1]);
        if(rank != -1) {
            resp.append(to_string(rank));
        }else{
            resp.append("(nil)");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zIncrBy(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        double new_score;
        bool score_valid;
        double inc = stringToDouble(args[1], score_valid);
        if(!score_valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        s = db->ZIncrBy(args[0], inc, args[2], new_score);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp = to_string(new_score);
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zRange(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int start;
        bool start_valid;
        start = stringToInt(args[1], start_valid);
        int end;
        bool end_valid;
        end = stringToInt(args[2], end_valid);
        if(!(start_valid && end_valid)){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        std::vector<std::string> vals;
        db->ZRange(args[0], start, end, vals);
        for(int i = 0; i < vals.size(); i++){
            resp.append(to_string(i+1) + ") ");
            resp.append(vals[i] + "\n");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zRevRange(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 3){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int start;
        bool start_valid;
        start = stringToInt(args[1], start_valid);
        int end;
        bool end_valid;
        end = stringToInt(args[2], end_valid);
        if(!(start_valid && end_valid)){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        std::vector<std::string> vals;
        db->ZRevRange(args[0], start, end, vals);
        for(int i = 0; i < vals.size(); i++){
            resp.append(to_string(i+1) + ") ");
            resp.append(vals[i] + "\n");
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zRem(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool suc;
        s = db->ZRem(args[0], args[1], suc);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), s.Message());
            return s;
        }
        resp = "(integer) ";
        resp.append(to_string(suc));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zGetByRank(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool rank_valid;
        int rank = stringToInt(args[1], rank_valid);
        if(!rank_valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        std::string member;
        bool suc = db->ZGetByRank(args[0], rank, member);
        if(suc){
            resp = member;
        }else{
            resp = "(nil)";
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zRevGetByRank(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool rank_valid;
        int rank = stringToInt(args[1], rank_valid);
        if(!rank_valid){
            resp = "syntax not correct";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        std::string member;
        bool suc = db->ZRevGetByRank(args[0], rank, member);
        if(suc){
            resp = member;
        }else{
            resp = "(nil)";
        }
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zKeyExist(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        bool exist = db->ZKeyExist(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(exist));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zClear(LightDB *db, std::vector <std::string> args, std::string &resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            return Status::OK();
        }
        int count;
        s = db->ZClear(args[0], count);
        if(!s.ok()){
            resp = Response::ResponseWrap(s.Code(), resp);
            return s;
        }
        resp.append(to_string(count));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    Status zExpire(LightDB *db, std::vector<std::string> args, std::string &resp){
        Status s;
        if(args.size() != 2){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int count = 0;
        bool suc;
        s = db->ZExpire(args[0], strtoull(args[1].c_str(), nullptr, 10), suc);
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

    Status zTTL(LightDB* db, std::vector<std::string> args, std::string& resp){
        Status s;
        if(args.size() != 1){
            resp = "wrong num of args";
            resp = Response::ResponseWrap(s.Code(), resp);
            return Status::OK();
        }
        int64_t ttl = db->ZTTL(args[0]);
        resp.append("(integer) ");
        resp.append(to_string(ttl));
        resp = Response::ResponseWrap(s.Code(), resp);
        return Status::OK();
    }

    void init_zset(){
        addExecCommand("zadd", zAdd);
        addExecCommand("zscore", zScore);
        addExecCommand("zcard", zCard);
        addExecCommand("zrank", zRank);
        addExecCommand("zrevrank", zRevRank);
        addExecCommand("zincrby", zIncrBy);
        addExecCommand("zrange", zRange);
        addExecCommand("zrevrange", zRevRange);
        addExecCommand("zrem", zRem);
        addExecCommand("zgetbyrank", zGetByRank);
        addExecCommand("zrevgetbyrank", zRevGetByRank);
        addExecCommand("zkeyexist", zKeyExist);
        addExecCommand("zclear", zClear);
        addExecCommand("zexpire", zExpire);
        addExecCommand("zttl", zTTL);
    }


}