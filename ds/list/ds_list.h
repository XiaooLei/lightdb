
#pragma once
#include <unordered_map>
#include <list>
#include <vector>
#include <mutex>
#include "../../include/status.h"

namespace lightdb{

// InsertOption insert option for LInsert.
enum InsertOption { Before = 1, After};
class List;

// List is the implementation of doubly linked list.
class List{
    public:
    // 每个key 对应一个链表
    std::unordered_map<std::string,std::list<std::string>> record;

    public:
    List() = default;

    // LPush insert all the specified values at the head of the list stored at key.
    // If key does not exist, it is created as empty list before performing the push operations.
    // return the number of elements in the list after the operation
    int LPush(const std::string& key, const std::string& val){
        if(!LKeyExist(key)){
            record.insert(make_pair("key", std::list<std::string>()));
        }
        record[key].push_front(val);
        return record[key].size();
    }

    bool LKeyExist(const std::string& key){
        if(record.find(key) == record.end()){
            return false;
        }
        return true;
    }
    
    // LPop removes and returns the first elements of the list stored at key.
    // if the list stored at key is empty, return false, else return true
    bool LPop(const std::string& key, std::string& val){
        if(!LKeyExist(key)){
            return false;
        }
        val = record[key].front();
        record[key].pop_front();
        if(record[key].empty()){
            record.erase(key);
        }
        return true; 
    }

    // RPush insert all the specified values at the tail of the list stored at key.
    // If key does not exist, it is created as empty list before performing the push operation.
    int RPush(const std::string& key, const std::string& val){
        if(!LKeyExist(key)){
            record.insert(std::make_pair("key", std::list<std::string>()));
        }
        record[key].push_back(val);
        return record[key].size();
    }

    // RPop removes the last elements of the list stored at key.
    // if the list stored at key is empty, return false, else return true 
    bool RPop(const std::string& key, std::string& val){
        if(!LKeyExist(key)){
            return false;
        }
        val = record[key].back();
        record[key].pop_back();
        if(record[key].empty()){
            record.erase(key);
        }
        return true;
    }

    // LIndex returns the element at index in the list stored at key.
    // The index is zero-based, so 0 means the first element, 1 the second element and so on.
    bool LIndex(const std::string& key, uint32_t index, std::string& val){
        if(!LKeyExist(key)){
            return false;
        }
        if(index<0 || index>=record[key].size()){
            return false;
        }
        auto it = record[key].begin();
        for(; index-->0; it++){
            //skip
        }
        val = *it;
        return true;
    }

    // LRem removes the first count occurrences of elements equal to element from the list stored at key.
    // The count argument influences the operation in the following ways:
    // count > 0: Remove elements equal to element moving from head to tail.
    // count < 0: Remove elements equal to element moving from tail to head.
    // count = 0: Remove all elements equal to element.
    // return the number of element removed in the list stored at the key
    int LRem(const std::string& key, const std::string& val, int count){
        int res = 0;
        if(record.find(key) == record.end()){
            return 0;
        }

        if(count == 0){
            for(auto it = record[key].begin(); it!=record[key].end(); it++){
                if(*it == val){
                    record[key].erase(it);
                    res++;
                }
            }
        }

        if(count>0){
            for(auto it = record[key].begin(); it!=record[key].end() && count>0;it++){
                if(*it == val){
                    record[key].erase(it);
                    count--;
                    res++;
                }
            }
        }

        if(count<0){
            // 从尾到头遍历，用反向迭代器
            for(auto it = record[key].rbegin(); it!=record[key].rend() && count<0; it++){
                if(*it == val){
                    record[key].erase(it.base());
                    count++;
                    res++;
                }
            }
        }
        
        return res;
    }

    // LLen returns the length of the list stored at key.
    // If key does not exist, it is interpreted as an empty list and 0 is returned.
    int LLen(const std::string& key){
        if(!LKeyExist(key)){
            return 0;
        }
        return record[key].size();
    }

    // LClear clear a specified key for List.
    // return the number of element of the list in this operation
    int LClear(const std::string& key){
        if(!LKeyExist(key)){
            return 0;
        }
        int res = record[key].size();
        record.erase(key);
        return res;
    }

    // LSet sets the list element at index to element.
    bool LSet(const std::string& key, int index, std::string val){
        if(!LKeyExist(key)){
            return false;
        }
        if(index<0 || index>= record[key].size()){
            return false;
        }
        int count = 0;
        auto it = record[key].begin();
        for(; count++<index; it++){
            //skip
        }
        *it = std::move(val);
        return true;
    }

    // LInsert inserts element in the list stored at key either before or after the reference value pivot.
    // if the pivot is in this list, insert and return the length of list after insertion, else if the pivot
    // does not exist in the list, return -1;
    int LInsert(const std::string& key, InsertOption option, const std::string& pivot, const std::string& val){
        std::list<std::string>::iterator pointer;
        bool flag = false;
        for(auto it = record[key].begin(); it!=record[key].end(); it++){
            if(*it == pivot){
                pointer = it;
                flag = true;
                break;
            }
        }
        if(!flag){
            return -1;
        }
        if(option == Before){
            record[key].insert(pointer,val);
        }else{
            pointer++;
            record[key].insert(pointer,val);
        }
        return record[key].size();
    }

    // LRange returns the specified elements of the list stored at key.
    // The offsets start and stop are zero-based indexes, with 0 being the first element of the list (the head of the list), 1 being the next element and so on.
    // These offsets can also be negative numbers indicating offsets starting at the end of the list.
    // For example, -1 is the last element of the list, -2 the penultimate, and so on.
    bool LRange(const std::string& key, int start, int end, std::vector<std::string>& vals){
        if(!LKeyExist(key)){
            return false;
        }
        int length = record[key].size();
        handleIndex(length, start, end);
        if(start > end || start>=length){
            return false;
        }
        auto it = record[key].begin();
        int count = 0;
        for(;count<start;it++){
            //skip;
            count++;
        }
        for(;count<=end;it++){
            count++;
            vals.push_back(*it);
        }
        return true;
    }

    // LTrim trim an existing list so that it will contain only the specified range of elements specified.
    // Both start and stop are zero-based indexes, where 0 is the first element of the list (the head), 1 the next element and so on.
    bool LTrim(const std::string& key, int start, int end){
        if(!LKeyExist(key)){
            return false;
        }
        int length = record[key].size();
        handleIndex(length,start, end);
        if(start > end || start >= length){
            record.erase(key);
            return true;
        }
        //printf("start:%d end:%d length:%d\n", start, end, length);
        if(end - start +1 < (length>>1) ){
            //小于一半
            std::list<std::string> newList;
            int index = 0;
            for(auto & it : record[key]){
                if(index<start){
                    continue;
                }else if(index >= start && index <= end){
                    newList.push_back(it);
                }else if(index > end){
                    break;
                }
            }
            record.erase(key);
            record.insert(make_pair(key, newList));
        }else{
            //大于一半
            int index = 0;
            std::vector<std::list<std::string>::iterator> vec;
            for(auto it = record[key].begin(); it!=record[key].end(); it++){
                if(index<start || index > end){
                    vec.push_back(it);
                }else if(index >= start && index <= end){
                    //skip
                }
                index++;
            }
            for(auto & it : vec){
                record[key].erase(it);
            }
        }
        return true;
    }
    
    static void handleIndex(int length, int& start, int& end){
        if (start < 0) {
		    start += length;
	    }

	    if (end < 0) {
		   end += length;
	    }   

	    if(start < 0){
		   start = 0;
	    }
	    if(end >= length) {
		  end = length - 1;
	    }
    }

};

struct ListIdx {
    List* indexes;
    std::mutex mtx;
    ListIdx(){
        indexes = new List();
    }
};

}

