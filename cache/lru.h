
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
    // 用map快速定位节点在链表中的位置
    unordered_map<string, DLinkedNode*> cache;
    // 用双向链表实现LRU
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
        // 如果 key 存在，先通过哈希表定位，再移到头部
        DLinkedNode* node = cache[key];
        moveToHead(node);
        value = node->value;
        return true;
    }
    
    void put(const string& key, const string& value) {
        if (!cache.count(key)) {
            // 如果 key 不存在，创建一个新的节点
            DLinkedNode* node = new DLinkedNode(key, value);
            // 添加进哈希表
            cache[key] = node;
            // 添加至双向链表的头部
            addToHead(node);// 新加入的节点是最新使用的，所以要移动到头部
            ++size;
            if (size > capacity) {
                // 如果超出容量，删除双向链表的尾部节点，因为尾部的节点最近最久未使用
                DLinkedNode* removed = removeTail();
                // 删除哈希表中对应的项
                cache.erase(removed->key);
                // 防止内存泄漏
                delete removed;
                --size;
            }
        }
        else {
            // 如果 key 存在，先通过哈希表定位，再修改 value，并移到头部
            DLinkedNode* node = cache[key];
            node->value = value;
            moveToHead(node);// 因为刚刚使用过了，所以要放在链表头部
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
