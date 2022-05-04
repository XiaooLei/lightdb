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
	StrSkipListNode (std::string key, Indexer val,int sz=32) : key(key), val(val),level(sz, nullptr) {}
	StrSkipListNode(const StrSkipListNode& node): key(node.key), val(node.val){}
};

class StrSkiplist {
private:
    StrSkipListNode *head, *tail;
    int level, length;

    StrSkipListNode* iteraterNodePointer;

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

    // 找到第一个key大于等于target key的节点，如果找不到，返回尾节点
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
        bool exist = p->key.compare(key) == 0;
        if(exist){
            val = p->val;
        }
        return exist;
    }
    
    // if key exist，update corresponding Indexer，or insert a StrSkipListNode
    void put(std::string key, Indexer val) {
        //if key exist
        StrSkipListNode* pointer = find(key);
        if(pointer && pointer->key.compare(key) == 0){
           pointer->val = val;
           return;
        }

        std::vector<StrSkipListNode *> update(MAXL);
        StrSkipListNode *p = head;
        for (int i = level - 1; i >= 0; --i) {
            while (p->level[i] && p->level[i]->key.compare(key) > 0) {
                p = p->level[i];
            }
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
        delete p;
        return true;
    }

    int randomLevel() {
        int lv = 1;
        while (lv < MAXL && (rand() & S) < PS) ++lv;
        return lv;
    }

    void Begin(){
        iteraterNodePointer = head->level[0];
    }

    void CurIterKeyValue(std::string& curKey, Indexer & curValue){
        curKey = iteraterNodePointer->key;
        curValue = iteraterNodePointer->val;
    }

    void Next(){
        iteraterNodePointer = iteraterNodePointer->level[0];
    }

    bool End(){
        return iteraterNodePointer->level[0] == nullptr;
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
