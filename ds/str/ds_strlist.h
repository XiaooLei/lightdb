#include <vector>
#include <string>
#include <mutex>
#include "../../storage/indexer.h"

namespace lightdb{
class StrSkiplist;
struct StrSkipListNode {
	std::string key;
    Indexer val;
	std::vector<StrSkipListNode *> level;
	StrSkipListNode (std::string key, Indexer val,int sz=32) : key(key), val(val),level(sz, nullptr) {
	    //printf("node created, level size %d \n", level.size());
	}
};

class StrSkiplist {
private:
    StrSkipListNode *head, *tail;
    int level, length;
public:
	static constexpr int MAXL = 32;
    static constexpr int P = 4;
    static constexpr int S = 0xFFFF;
    static constexpr int PS = S / 4;

    StrSkiplist() {
        level = length = 0;
        tail = new StrSkipListNode("",  Indexer());
        head = new StrSkipListNode("", Indexer());
        for (int i = 0; i < MAXL; ++i) { 
        	head->level[i] = tail;
        }
    }

    StrSkipListNode* find(std::string key) {
        StrSkipListNode *p = head;
        for (int i = level - 1; i >= 0; --i) {
            while (p->level[i] && p->level[i]->key.compare(key) > 0) {
                p = p->level[i];
            }
        }
        p = p->level[0];
        return p;
    }
    
    bool get(std::string key, Indexer& val) {
        StrSkipListNode *p = find(key);
        //printf("key: %s \n", key.c_str());
        //printf("p-> key:%s \n", p->key.c_str());
        bool exist = p->key.compare(key) == 0;
        if(exist){
            val = p->val;
            //printf("buttom get, key:%s fileId:%d, offset:%d  p->value:%s \n", key.c_str(), p->val.fileId, p->val.offset, p->val.meta->value.c_str());
        }
        return exist;
    }
    
    void put(std::string key, Indexer val) {
        //if key exist
        StrSkipListNode* pointer = find(key);
        if(pointer && pointer->key.compare(key) == 0){
           pointer->val = val;
           return;
        }

        std::vector<StrSkipListNode *> update(MAXL);
        //printf("update size %d \n", update.size());
        StrSkipListNode *p = head;
        //printf("level:%d \n",level);
        for (int i = level - 1; i >= 0; --i) {
            //printf("aaa\n");
            //printf("level size:%d, i:%d \n",p->level.size(),i);
            while (p->level[i] && p->level[i]->key.compare(key) > 0) {
                //printf("level size:%d, i:%d \n",p->level.size(),i);
                p = p->level[i];
            }
            //printf("i :%d \n",i);
            update[i] = p;
        }
        int lv = randomLevel();
        if (lv > level) {
            lv = ++level;
            update[lv - 1] = head; 
        }
        StrSkipListNode *newNode = new StrSkipListNode(key, val, lv);
        for (int i = lv - 1; i >= 0; --i) {
            p = update[i];
            newNode->level[i] = p->level[i];
            p->level[i] = newNode;
        }
        ++length;
    }
    
    bool erase(std::string key) {
        std::vector<StrSkipListNode *> update(MAXL + 1);
        StrSkipListNode *p = head;
        for (int i = level - 1; i >= 0; --i) {
            while (p->level[i] && p->level[i]->key.compare(key) > 0) {
                p = p->level[i];
            }
            update[i] = p;
        }
        p = p->level[0];
        if (p->key.compare(key)!=0) return false;
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

    int randomLevel() {
        int lv = 1;
        while (lv < MAXL && (rand() & S) < PS) ++lv;
        return lv;
    }
};

struct StrIdx{
    StrSkiplist* indexes;
    std::mutex mtx;

    StrIdx(){
        indexes = new StrSkiplist();
    }
};

}