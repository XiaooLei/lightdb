
#pragma once
#include "../../include/status.h"
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include <cstdio>
#include <climits>

namespace lightdb{

struct SkipListNode {
	double score;
    std::set<std::string> members;// member是string的集合
	std::vector<SkipListNode *> level;
	explicit SkipListNode (double _score, int sz=32) : score(_score), level(sz, nullptr) {}
};


class Skiplist {
private:
    SkipListNode *head, *tail;
    int level, length;// level表示当前跳表的层数，不包含头节点的层数，因为是从第0层开始算的，所以从第0层至level - 1层才有效

    SkipListNode* curNodePtr;
    std::set<std::string>::iterator curMemberPtr;
public:
	static constexpr int MAXL = 32;
    static constexpr int S = 0xFFFF;
    static constexpr int PS = S / 4;

    Skiplist():curNodePtr(nullptr){
        level = length = 0;
        tail = new SkipListNode(INT_MAX, 0);
        head = new SkipListNode(INT_MAX);// 头结点有32层
        for (int i = 0; i < MAXL; ++i) { 
        	head->level[i] = tail;
        }
    }

    // 从跳表中找出第一个分数大于等于score的节点，如果不存在，返回tail
    SkipListNode* find(double score) {
        SkipListNode *p = head;
        for (int i = level - 1; i >= 0; --i) {
            while (p->level[i] && p->level[i]->score < score) {
                p = p->level[i];
            }
        }
        p = p->level[0];
        return p;
    }
    
    SkipListNode* add(double score, const std::string& member) {
        SkipListNode* p = find(score);
        if(p->score==score){
            p->members.insert(member);
            return p;
        }

        std::vector<SkipListNode *> update(MAXL);
        p = head;
        for (int i = level - 1; i >= 0; --i) {
            while (p->level[i] && p->level[i]->score < score) {
                p = p->level[i];
            }
            update[i] = p;
        }
        int lv = randomLevel();
        if (lv > level) {
            for (int i = level; i < lv; i++) {
                update[i] = head;
            }
            level = lv;
        }
        auto newNode = new SkipListNode(score, lv);
        newNode->members.insert(member);
        for (int i = lv - 1; i >= 0; --i) {
            p = update[i];
            newNode->level[i] = p->level[i];
            p->level[i] = newNode;
        }
        ++length;
        return newNode;
    }
    

    bool erase(double score, const std::string& member) {
        std::vector<SkipListNode *> update(MAXL + 1);
        SkipListNode *p = head;
        for (int i = level - 1; i >= 0; --i) {
            while (p->level[i] && p->level[i]->score < score) {
                p = p->level[i];
            }
            update[i] = p;
        }
        p = p->level[0];
        if (p->score != score) return false;
        
        if(p->members.find(member) == p->members.end()){
            return false;
        }

        p->members.erase(member);

        if(!p->members.empty()){
            return true;
        }

        for (int i = 0; i < level; ++i) {
            if (update[i]->level[i] != p) {
                break;
            }
            update[i]->level[i] = p->level[i];
        }
        while (level > 0 && head->level[level - 1] == tail) --level;
        --length;
        return true;
    }

    static int randomLevel() {
        int lv = 1;
        while (lv < MAXL && (rand() & S) < PS) { ++lv; }
        return lv;
    }

    int rank(const std::string& member,double target_score){
        SkipListNode* p = head;
        int rank = 0;
        while(p&&p->level[0]->score<target_score){
            rank += p->level[0]->members.size();
            p = p->level[0];
        }
        p = p->level[0];
        for(const auto & it : p->members){
            if(it == member){
                break;
            }
            rank++;
        }
        return rank;
    }

    bool getByRank(int rank, std::string& member){
        if(rank<0){
            return false;
        }

        SkipListNode* p = head;
        while(p->level[0] && p->level[0]->members.size() <= rank){
            rank-=p->level[0]->members.size();
            p=p->level[0];
        }
        if(!p->level[0] && rank>=0){
            return false;
        }
        p = p->level[0];
        auto it = p->members.begin();
        for(; rank>0; rank--){
            it++;
        }
        member = *it;
        return true;
    }

    //iterator
    double CurScore(){
        return curNodePtr->score;
    }

    // 当前节点所指向的string或者下一个节点的第一个string
    std::string CurMember(){
        if(curMemberPtr == curNodePtr->members.end()){
            curNodePtr = curNodePtr->level[0];
            curMemberPtr = curNodePtr->members.begin();
        }
        return *curMemberPtr;
    }

    void Begin(){
        curNodePtr = head->level[0];
        curMemberPtr = curNodePtr->members.begin();
    }

    void Next(){
        if(curMemberPtr == curNodePtr->members.end()){
            curNodePtr = curNodePtr->level[0];
            curMemberPtr = curNodePtr->members.begin();
        }else{
            curMemberPtr++;
        }
    }

    bool End(){
        return curNodePtr->level[0] == nullptr;
    }

};


}// namespace lightdb
