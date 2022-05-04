
#include <string>
#include <unordered_map>
using namespace std;
namespace lightdb{

struct DLinkedNode {
    string key, value;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode(): key(""), value(""), prev(nullptr), next(nullptr) {}
    DLinkedNode(string _key, string _value): key(_key), value(_value), prev(nullptr), next(nullptr) {}
};

class LRUCache {
private:
    unordered_map<string, DLinkedNode*> cache;
    DLinkedNode* head;
    DLinkedNode* tail;
    int size;
    int capacity;

public:
    LRUCache(int _capacity): size(0), capacity(_capacity) {
        // 使用伪头部和伪尾部节点
        head = new DLinkedNode();
        tail = new DLinkedNode();
        head->next = tail;
        tail->prev = head;
    }
    
    void remove(const string& key) {
        if (cache.find(key) == cache.end()) {
            return;
        }
        
        removeNode(cache[key]);
        return;
    }


    bool get(const string& key, string& value){
        if (cache.find(key) == cache.end()) {
            return false;
        }
        DLinkedNode* node = cache[key];
        moveToHead(node);
        value = node->value;
        return true;
    }
    
    void put(const string& key, const string& value) {
        if (!cache.count(key)) {
            DLinkedNode* node = new DLinkedNode(key, value);
            cache[key] = node;
            addToHead(node);
            ++size;
            if (size > capacity) {
                DLinkedNode* removed = removeTail();
                cache.erase(removed->key);
                delete removed;
                --size;
            }
        }
        else {
            DLinkedNode* node = cache[key];
            node->value = value;
            moveToHead(node);
        }
    }

    void addToHead(DLinkedNode* node) {
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
    }
    
    void removeNode(DLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void moveToHead(DLinkedNode* node) {
        removeNode(node);
        addToHead(node);
    }

    DLinkedNode* removeTail() {
        DLinkedNode* node = tail->prev;
        removeNode(node);
        return node;
    }
}; 


}//namespace lightdb
