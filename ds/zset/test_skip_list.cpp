
#include "skiplist.h"
#include <iostream>
#include <cassert>

using namespace lightdb;

int main(){
    Skiplist skiplist;
    
    skiplist.add(1,"xiaolei");
    assert(skiplist.search(1,"xiaolei") == true);
    cout<<"valid test skiplist add, search"<<endl;

    bool ret = skiplist.erase(1,"xiaolei");
    if(!ret){
        cout<<"skiplist erase failed"<<endl;
    }
    
    assert(skiplist.search(1,"xiaolei") == false);
    cout<<"valid test skiplist erase!" <<endl;

    skiplist.add(1,"aaa");
    skiplist.add(2,"bbb");
    skiplist.add(3,"ccc");
    skiplist.add(3,"ddd");
    string val;
    skiplist.getByRank(0,val);
    cout<<"rank 0 :"<<val <<endl;
    skiplist.getByRank(2,val);
    cout<<"rank 2 :"<<val <<endl;
    skiplist.getByRank(3,val);
    cout<<"rank 3 :"<<val <<endl;
    return 0;
}