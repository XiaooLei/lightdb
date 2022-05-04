
#pragma once
#include "skiplist.h"
#include <unordered_map>
#include <mutex>

namespace lightdb{


class SortedSet{
public:
    std::unordered_map<std::string,Skiplist> record;
    std::unordered_map<std::string,std::unordered_map<std::string,SkipListNode*>> dict;

    public:
    void ZAdd(const std::string& key, double score, const std::string& member){
        if(ZKeyExist(key)){
            record.insert(make_pair(key, Skiplist()));
            dict.insert(make_pair(key,std::unordered_map<std::string,SkipListNode*>()));
        }
        SkipListNode* node = record[key].add(score,member);
        dict[key].insert(make_pair(member,node));
    }

    bool ZKeyExist(const std::string& key){
        if(record.find(key)==record.end()){
            return false;
        }
        return true;
    }

    //if the key or the member does not exist, return false
    bool ZScore(const std::string& key, const std::string& member, double& score){
        if(!ZKeyExist(key)){
            return false;
        }
        if(dict[key].find(member) == dict[key].end()){
            return false;
        }
        SkipListNode* node = dict[key][member];

        score = node->score;
        return true;
    }

    int ZCard(const std::string& key){
        if(!ZKeyExist(key)){
            return 0;
        }
        return dict[key].size();
    }

    int ZRank(const std::string& key, const std::string& member){
        if(!ZKeyExist(key)){
            return -1;
        }
        if(dict[key].find(member) == dict[key].end()){
            return -1;
        }
        double member_score = dict[key][member]->score;
        return record[key].rank(member,member_score);
    }

    int ZRevRank(const std::string& key, const std::string& member){
        if(!ZKeyExist(key)){
            return -1;
        }
        if(dict[key].find(member) == dict[key].end()){
            return -1;
        }
        double member_score = dict[key][member]->score;
        return dict.size() - record[key].rank(member, member_score) -1;
    }

    bool ZRem(const std::string& key, const std::string& member){
        Status s;
        if(!ZKeyExist(key)){
            return false;
        }
        double score;
        bool exist = ZScore(key,member,score);
        if(!exist){
            return false;
        }
        record[key].erase(score,member);
        dict[key].erase(member);
        return true;
    }

    double ZIncrBy(const std::string& key, double increment, const std::string& member){
        Status s;
        double old_score = 0;
        bool exist = ZScore(key, member, old_score);
        if(exist) {
            ZRem(key,member);
        }

        double new_score = old_score + increment;

        ZAdd(key, new_score, member);

        return new_score;
    }

    void ZRevRange(const std::string& key, int start, int end, std::vector<std::string>& vals) {
        int size = dict[key].size();
        start = size - end -1;
        end = size - start -1;
        ZRange(key, start, end, vals);
    }




    void ZRange(const std::string& key, int start, int end, std::vector<std::string>& vals){
        std::string start_member;
        bool get = record[key].getByRank(start, start_member);
        if(!get){
            return;
        }
        std::string end_member;
        get = record[key].getByRank(end,end_member);
        if( !get ){
            return;
        }
        SkipListNode* start_node = dict[key][start_member];
        auto it = start_node->members.begin();
        for(; *it != start_member; it++){
            //skip;
        }
        int count = end-start+1;

        while(count>0 && it!=start_node->members.end()){
            //printf("first_node: %s \n",it->c_str());
            vals.push_back(*it);
            it++;
            count--;
        }

        SkipListNode* p = start_node;
        for(;p->level[0]&&count>0;){
            for(auto it = p->level[0]->members.begin(); it!=p->level[0]->members.end() && count>0; it++){
                vals.push_back(*it);
                count--;
            }
            p = p->level[0];
        }
    }

    bool ZGetByRank(const std::string& key, int rank, std::string& member){
        if(!ZKeyExist(key)){
            return false;
        }
        return record[key].getByRank(rank,member);
    }

    bool ZRevGetByRank(const std::string& key, int revRank, std::string& member){
        if(!ZKeyExist(key)){
            return false;
        }
        int rank = dict[key].size() - revRank -1;
        return record[key].getByRank(rank,member);
    }

    int ZClear(const std::string& key){
        if(!ZKeyExist(key)){
            return 0;
        }
        int count = ZCard(key);
        record.erase(key);
        dict.erase(key);
        return count;
    }

};

struct SortedSetIdx{
    SortedSet* indexes;
    std::mutex mtx;
    SortedSetIdx(){
        indexes = new SortedSet();
    }
};


}// namespace lightdb;
