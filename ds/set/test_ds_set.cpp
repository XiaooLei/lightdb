
#include "ds_set.h"
#include <iostream>
#include <cassert>
#include <vector>

using namespace lightdb;
int main(){

    Status s;
    //test SAdd 
    Set set;
    set.SAdd("Test_SAdd","111");
    set.SAdd("Test_SAdd","222");
    set.SAdd("Test_SAdd","333");
    assert(set.SCard("Test_SAdd") == 3);
    std::cout<< "Valid test SAdd!" <<std::endl;

    //test SPop
    set.SAdd("Test_SPop","111");
    set.SAdd("Test_SPop","222");
    set.SAdd("Test_SPop","333");
    std::string val;
    s = set.SPop("Test_SPop", val);
    if( !s.ok() ){
        std::cout<<"Spop failed, code:"<< s.Code() << std::endl;
    }

    assert(set.SCard("Test_SPop") == 2);
    std::cout<< "Valid test Spop !" <<std::endl;

    //test SIsMember

    set.SAdd("Test_SIsMember","111");
    set.SAdd("Test_SIsMember","222");
    assert(set.SIsMember("Test_SIsMember","111") == true);
    assert(set.SIsMember("Test_SIsMember","333") == false);
    std::cout<< "Valid test SIsMember !" <<std::endl;

    //test SRem
    set.SAdd("Test_SRem", "111");
    set.SAdd("Test_SRem", "222");
    int count;
    set.SRem("Test_SRem", "111", count);
    assert(set.SIsMember("Test_SRem","111") == false);
    std::cout<< "valid test SRem !" << std::endl;

    //test SMove
    set.SAdd("Test_SMove_src","111");
    set.SAdd("Test_SMove_dst","222");
    s = set.SMove("Test_SMove_src","Test_SMove_dst","111");
    if(!s.ok()){
        std::cout<<"SMove failed , code:" << s.Code()<< std::endl;
    }
    assert(set.SCard("Test_SMove_src") == 0);
    assert(set.SCard("Test_SMove_dst") == 2);
    std::cout<< "valid test SMove !" <<std::endl;

    //test SMembers
    set.SAdd("Test_SMembers","aaa");
    set.SAdd("Test_SMembers","bbb");
    std::vector<std::string> members;
    set.SMembers("Test_SMembers",members);
    /*
    for(auto it = members.begin(); it!=members.end(); it++){
        cout<<*it<<endl;
    }*/

    //test SUnion
    set.SAdd("Test_SUnion_a","aaa");
    set.SAdd("Test_SUnion_b","bbb");
    std::vector<std::string> keys;
    keys.push_back("Test_SUnion_a");
    keys.push_back("Test_SUnion_b");
    std::vector<std::string> union_vals;
    set.SUnion(keys,union_vals);
    // for(std::string it : union_vals){
    //     cout<< it <<endl;
    // }
    
    //test
    return 0;
}