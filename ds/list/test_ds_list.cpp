
#include <iostream>
#include "ds_list.h"
#include <cassert>

using namespace lightdb;
int test_push_pop(){
    lightdb::Status s;
    lightdb::List list;
    list.LPush("111","aaa");
    list.LPush("111","bbb");
    list.LPush("111","ccc");
    assert(list.LLen("111") == 3);
    std::string value;
    list.LPop("111",value);
    std::cout<<value<<std::endl;
    list.LPop("111",value);
    std::cout<<value<<std::endl;
    list.LPop("111",value);
    std::cout<<value<<std::endl;
    assert(list.LLen("111") == 0);

    list.RPush("222","xiaolei");
    list.RPush("222","lijunbiao");
    s = list.LClear("222");
    assert(s.ok());    
    int len = list.LLen("222");
    assert(len<0);

    //test LRange;
    list.LPush("333","guangzhou");
    list.LPush("333","changsha");
    list.LPush("333","shanghai");
    std::vector<std::string> cities;
    list.LRange("333",0,2, cities);
    assert(cities.size() == 2);
    assert(cities[0].compare("shanghai") == 0);
    std::cout<<"valid test LRange!"<<std::endl;

    //test LSet
    s = list.LSet("333",1,"hangzhou");
    assert(s.ok());
    std::string tmp;
    list.LIndex("333",1,tmp);
    assert(tmp.compare("hangzhou") == 0);
    std::cout<<"valid test LSet!"<<std::endl;

    //test LInsert
    list.RPush("LInsert","111");
    list.RPush("LInsert","222");
    list.RPush("LInsert","333");
    s = list.LInsert("LInsert", lightdb::Before, 3, "alibaba");
    if(!s.ok()){
        printf("LInsert Failed, code:%d\n",s.Code());
    }
    s = list.LIndex("LInsert",3,tmp);
    if(!s.ok()){
        printf("LIndex failed, code:%d\n",s.Code());
    }
    assert(tmp.compare("alibaba")==0);
    std::cout<<"valid test LInsert"<<std::endl;
    
    return 0;
}


void test_LInsert(){
    lightdb::Status s;
    lightdb::List list;
    list.LPush("key","aaa");
    list.LPush("key","bbb");
    list.LPush("key","ccc");
    list.LInsert("key", lightdb::Before, "ccc", "111");
    std::vector<std::string> vec;
    std::string str;
    s = list.LPop("key", str);
    assert(s.ok());
    // while(list.LLen("key")>0){
    //     s = list.LPop("key", str);
    //     assert(s.ok());
    //     cout<<str<<endl;
    // }
    assert(str.compare("111") == 0);

}

void test_LTrim(){
    lightdb::Status s;
    lightdb::List list;
    list.LPush("key","aaa");
    list.LPush("key","bbb");
    list.LPush("key","ccc");
    list.LPush("key","ddd");
    s = list.LTrim("key", 1, 2);
    assert(list.LLen("key") == 2);
    std::string str;
    while(list.LLen("key")>0){
        s = list.LPop("key", str);
        assert(s.ok());
        std::cout<<str<<std::endl;
    }
    assert(s.ok());
    std::cout<<list.LLen("key")<<std::endl;
    assert(list.LLen("key") == 0);
    
}

void test_LRange(){
    lightdb::Status s;
    lightdb::List list;
    std::string str;
    list.LPush("key","aaa");
    list.LPush("key","bbb");
    list.LPush("key","ccc");
    list.LPush("key","ddd");
    std::vector<std::string> vals;
    list.LRange("key", 1, 2, vals);
    for(auto it = vals.begin(); it!=vals.end(); it++){
        std::cout<<*it<<std::endl;
    }
    
}


int main(){
    //test_LInsert();
    //test_LTrim();
    test_LRange();
    return 0;
}
