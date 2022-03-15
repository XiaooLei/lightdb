#include "ds_hash.h"
#include <stdio.h>
#include <iostream>

int test_HKeys(){
    lightdb::Hash hash;
    lightdb::Status s;
    s = hash.HSet("aaa","bbb","ccc");
    if(!s.ok()){
        printf("HSet failed,code %d",s.Code());
    }
    s = hash.HSet("aaa","111","222");
    if(!s.ok()){
        printf("HSet failed,code %d",s.Code());
    }
    std::vector<std::string> vec;
    hash.HKeys("aaa",vec);
    for(auto it = vec.begin(); it!= vec.end(); it++){
        std::cout<< *it << std::endl;
    }
    
    //test HVals
    std::vector<std::string> values;
    hash.HVals("aaa",values);
    for(auto it = values.begin(); it!=values.end();it++){
        std::cout<< *it <<std::endl;
    }
}

int test_get_set(){
    lightdb::Status s;
    lightdb::Hash hash;
    hash.HSet("aaa","bbb","ccc");
    std::string value;
    hash.HGet("aaa","bbb",value);
    std::cout<<value<<std::endl;
    s = hash.HDel("aaa","bbb");
    if(!s.ok()){
        printf("HDel failed,code:%d \n",s.Code());
    }
    auto new_get = hash.HGet("aaa","bbb",value);
    if(!s.ok()){
        printf("HGet failed,code:%d \n",s.Code());
    }
}



int main(){
    //test_HKeys();
    test_get_set();
    return 0;
}
