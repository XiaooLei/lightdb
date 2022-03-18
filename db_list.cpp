#include "lightdb.h"

namespace lightdb{

    // LPush insert all the specified values at the head of the list stored at key.
    // If key does not exist, it is created as empty list before performing the push operations.
    Status LightDB::LPush(std::string key, std::string value, int& length){
        //store
        Status s;
        s = CheckKeyValue(key, value);
        if(!s.ok()){
            return s;
        }
        CheckExpired(key, List);
        //update index
        listIdx.indexes->LPush(key,value);

        //store
        Entry* e = Entry::NewEntryNoExtra(key, value, List, ListLPush);
        s = store(e);
        if(!s.ok()){
            return s; 
        }
        length = LLen(key);
        return Status::OK();
    }

    // RPush insert all the specified values at the tail of the list stored at key.
    // If key does not exist, it is created as empty list before performing the push operation.
    Status LightDB::RPush(std::string key, std::string value, int& length){
        Status s;

        s = CheckKeyValue(key, value);
        if(!s.ok()){
            return s;
        }
        CheckExpired(key, List);
        //update index
        length = listIdx.indexes->RPush(key, value);
        //store;
        Entry* e = Entry::NewEntryNoExtra(key, value, List, ListRPush);
        s = store(e);
        if(!s.ok()){
            return s; 
        }
        //update index
        length = LLen(key);
        return Status::OK();
    }

    // LPop removes and returns the first elements of the list stored at key.
    Status LightDB::LPop(std::string key, std::string& val, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return s;
        }
        CheckExpired(key, List);
        suc = listIdx.indexes->LPop(key, val);
        //store;
        Entry* e = Entry::NewEntryNoExtra(key, val, List, ListLPop);
        s = store(e);
        if(!s.ok()){
            return s; 
        }
        return Status::OK();
    }

    // Removes and returns the last elements of the list stored at key.
    Status LightDB::RPop(std::string key, std::string& val, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return s;
        }
        CheckExpired(key, List);
        suc = listIdx.indexes->RPop(key, val);
        //store
        Entry* e = Entry::NewEntryNoExtra(key, val, List, ListRPop);
        s = store(e);
        if(!s.ok()){
            return s; 
        }
        return Status::OK();
    }

    // LIndex returns the element at index  in the list stored at key.
    // The index is zero-based, so 0 means the first element, 1 the second element and so on.
    bool LightDB::LIndex(std::string key, uint32_t index, std::string& val){
        Status s;

        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;
        }

        bool expired = CheckExpired(key, List);
        if(expired){
            return false;
        }

        return listIdx.indexes->LIndex(key, index, val);
    }

    // LRem removes the first count occurrences of elements equal to element from the list stored at key.
    // The count argument influences the operation in the following ways:
    // count > 0: Remove elements equal to element moving from head to tail.
    // count < 0: Remove elements equal to element moving from tail to head.
    // count = 0: Remove all elements equal to element.   
    Status LightDB::LRem(std::string key, std::string value, int count, int& removed){
        Status s;
        s = CheckKeyValue(key, value);
        if(!s.ok()){
            return s;
        }
        bool expired = CheckExpired(key, List);
        if(expired){
            removed = 0;
            return Status::OK();
        }
        removed = listIdx.indexes->LRem(key, value, count);
        //store;
        Entry* e = Entry::NewEntryNow(key, value, to_string(count), List, ListLRem);
        s = store(e);
        if(!s.ok()){
            return s; 
        }
        return Status::OK();
    }

    // LInsert inserts element in the list stored at key either before or after the reference value pivot.
    Status LightDB::LInsert(std::string key, InsertOption option, std::string pivot, std::string val, int& count){
        Status s;
        s = CheckKeyValue(key, pivot);
        if(!s.ok()){
            return s;
        }
        s = CheckKeyValue(key, val);
        if(!s.ok()){
            return s;
        }
        bool expired = CheckExpired(key, List);
        if(expired){
            count = 0;
            return Status::OK();
        }
        count = listIdx.indexes->LInsert(key, option, pivot, val);
        //store;
        std::string extra;
        extra.append(pivot);
        extra.append(ExtraSeparator);
        extra.append(to_string(option));
        Entry* e = Entry::NewEntryNow(key, val, extra, List, ListLInsert);
        s = store(e);
        if(!s.ok()){
            return s; 
        }
        return Status::OK();
    }

    // LSet sets the list element at index to element.
    // returns whether is successful.
    Status LightDB::LSet(std::string key, int idx, std::string value, bool& suc){
        Status s;
        s = CheckKeyValue(key, value);
        if(!s.ok()){
            return s;
        }
        bool expired = CheckExpired(key, List);
        if(expired){
            suc = false;
            return Status::OK();
        }        
        suc = listIdx.indexes->LSet(key, idx, value);
        if(!suc){
            return Status::OK();
        }
        std::string extra = to_string(idx);
        Entry* e = Entry::NewEntryNow(key, value, extra, List, ListLSet);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // LTrim trim an existing list so that it will contain only the specified range of elements specified.
    // Both start and stop are zero-based indexes, where 0 is the first element of the list (the head), 1 the next element and so on.
    Status LightDB::LTrim(std::string key, int start, int end, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return Status::OK();
        }
        bool expired = CheckExpired(key, List);
        if(expired){
            return Status::OK();
        }
        suc = listIdx.indexes->LTrim(key, start, end);

        std::string extra;
        extra.append(to_string(start));
        extra.append(ExtraSeparator);
        extra.append(to_string(end));
        Entry* e = Entry::NewEntryNow(key, "", extra, List, ListLTrim);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // LRange returns the specified elements of the list stored at key.
    // The offsets start and stop are zero-based indexes, with 0 being the first element of the list (the head of the list), 1 being the next element and so on.
    // These offsets can also be negative numbers indicating offsets starting at the end of the list.
    // For example, -1 is the last element of the list, -2 the penultimate, and so on.
    Status LightDB::LRange(std::string key, int start, int end, std::vector<std::string>& vals, bool& suc){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return s;
        }
        suc = listIdx.indexes->LRange(key, start, end, vals);
        return Status::OK();
    }

    bool LightDB::LKeyExist(std::string key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return false;
        }
        return listIdx.indexes->LKeyExist(key);
    }

    int LightDB::LLen(std::string key){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            return 0;
        }
        return listIdx.indexes->LLen(key);
    }

    // LClear clear a specified key for List.
    // return the number of eliment of the list in this operation
    Status LightDB::LClear(std::string key, int& count){
        Status s;
        s = CheckKeyValue(key, "");
        if(!s.ok()){
            count = 0;
            return Status::OK();            
        }
        bool expired = CheckExpired(key, List);
        if(expired){
            count = 0;
            return Status::OK();
        }
        count = listIdx.indexes->LClear(key);

        Entry* e = Entry::NewEntryNoExtra(key, "", List, ListLClear);
        s = store(e);
        if(!s.ok()){
            return s;
        }
        return Status::OK();
    }

    // LExpire set expired time for a specified key of List.
    Status LightDB::LExpire(std::string key, uint64_t duration, bool& suc){
        Status s;
        if(!LKeyExist(key)){
            suc = false;
            return Status::KeyNotExist();
        }
        uint64_t deadline = getCurrentTimeStamp() + duration * 1000;
        Entry* e = Entry::NewEntryWithExpire(key, "", deadline, List, ListLExpire);
        s = store(e);
        if(!s.ok()){
            return s;
        }        
        expires[List][key] = deadline;
        suc = true;
        return Status::OK();
    }

    // LTTL return time to live.
    int64_t LightDB::LTTL(std::string key){
        bool expired = CheckExpired(key, List);
        if(expired){
            return -2;
        }
        if(expires[List].find(key) == expires[List].end()){
            return -2;
        }
        uint64_t deadline = expires[List][key];
        return (deadline - getCurrentTimeStamp()) / 1000;
    }
    


}//namespace lightdb