#include "lru.h"
#include <iostream>
#include <cassert>
using namespace lightdb;
using namespace std;
int main(){
    LRUCache cache(2);
    cache.put("111","aaa");
    cache.put("222","bbb");
    string val;
    bool get = cache.get("111",val);
    assert(get == true);
    cout<< val <<endl;
    cache.put("333","ccc");
    get = cache.get("222", val);
    assert(get == false);

    return 0;
}