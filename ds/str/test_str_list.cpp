

#include "ds_strlist.h"
#include <iostream>
using namespace std;
using namespace lightdb;
int main(){
    StrSkiplist strlist;
    Indexer idx1 = {nullptr, 1, 1};
    Indexer idx2 = {nullptr, 2, 2};
    strlist.put("aaa",idx1);
    strlist.put("bbb",idx2);
    Indexer val;
    bool get = strlist.get("bbb", val);
    cout<<val.fileId<<endl;
    return 0;
}