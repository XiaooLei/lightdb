
#include "lightdb.h"
#include <iostream>
#include <stdio.h>
#include <cassert>
using namespace std;
using namespace lightdb;


void test_str(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    string value;
    bool suc;
    s = lightdb->Get("kaaa", value, suc);
    if(!s.ok()){
        s.Report();
    }
    assert( suc );
    cout<< value <<endl;
    s = lightdb->Remove("kaaa", suc);
    assert( suc );

}

void TestStr_MSet(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);

    vector<std::string> values = {"key_lijunbiao", "dongguanligong", "key_zhengzhifeng", "changshaligong"};
    s = lightdb->MSet( values);
    //expected to be successful
    if(!s.ok()){
        s.Report();
    }

    vector<std::string> values2 = {
            "1", "2", "3"
    };
    //s = lightdb->MSet(values);
    //expected to fail;
    if(!s.ok()){
        s.Report();
    }
}


void TestStrMGet(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);

    vector<string> values;
    vector<bool> sucs;
    vector<string> keys = {"key_xiaolei", "key_lijunbiao", "key_zhengzhifeng", "key_lijiawei"};
    s = lightdb->MGet(keys, values, sucs);
    for(bool suc : sucs){
        cout<< "suc:" << suc << endl;
    }
    for(auto value : values){
        cout<< "value:" <<value <<endl;
    }
}

void TestStrRemove(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    bool suc;
    s = lightdb->Remove("key_lijunbiao", suc);
    cout << "suc:" << suc << endl;

}

void TestStr_SetNx(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    bool suc;
    s = lightdb->StrSetNx("key_xiaolei", "jinandaxue", suc);
    if(!s.ok()){
        printf("code: %d \n", s.Code());
    }

    printf("suc :%d \n", suc);
}


void TestHash_HSet(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    int res;
    s = lightdb->HSet("hash_key1", "hash_field1", "hash_value1", res);
    if(!s.ok()){
        s.Report();
    }
    std::string value;

    printf("res:%d \n",res);

}

void TestHash_HGet(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    std::string value;
    bool get = lightdb->HGet("hash_key1", "hash_field1", value);
    assert(get);
    printf("value:%s \n", value.c_str());
}

void TestStrAppend(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    int length;
    s = lightdb->Append("key_xiaolei", "suffix", length);
    cout << "length:" << length <<endl;
    if(!s.ok()){
        printf("code: %d \n", s.Code());
    }
    std::string value;
    bool suc;
    s = lightdb->Get("key_xiaolei", value, suc);
    cout << "suc:" << suc <<endl;
    cout << "value:" << value <<endl;
}


void TestGetSet(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    std::string oldValue;
    bool suc;
    s = lightdb->GetSet("key_xiaolei", oldValue, "jinanUniversity", suc);
    if(!s.ok()){
        printf("code:%d \n",s.Code());
    }
    cout << "old value:" << oldValue <<endl;
    cout<< "suc:" << suc <<endl;
}



void test_open(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    //test str
    s = lightdb->StrSet("kxiaolei", "kjnu");
    if(!s.ok()){
        s.Report();
    }

//    s = lightdb->StrSet("kccc","kddd");
//    if(!s.ok()){
//        s.Report();
//    }

}


void Test_StrExist(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);

    bool exist = lightdb->StrExist("key_xiaolei");
    cout << "exist:" << exist << endl;
}

/*************************  TestList  ************************/

void TestListLPush(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    int length;
    s = lightdb->LPush("list_key_a", "list_value_c", length);
    if(!s.ok()){
        printf("code: %d\n", s.Code());
    }
    printf("length:%d \n", length);
}

void TestListLLen(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    int length  = lightdb->LLen("list_key_a");
    printf("length :%d \n", length);

}

void TestListLPop(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    std::string value;
    bool suc;
    while(lightdb->LLen("list_key_a") > 0){
        lightdb->LPop("list_key_a", value, suc);
        printf("suc %d, value:%s \n", suc, value.c_str());
    }

}


void TestListLIndex(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    int length;
    s = lightdb->RPush("list_key_a", "list_value_a", length);
    s = lightdb->RPush("list_key_a", "list_value_b", length);
    s = lightdb->RPush("list_key_a", "list_value_c", length);
    std::string value;
    bool get = lightdb->LIndex("list_key_a", 1, value);
    printf("value :%s \n", value.c_str());
}


void TestMerge(){
    LightDB* lightdb = new LightDB();
    Status s;
    Config config = Config::DefaultConfig();
    lightdb->Open(&config);
    while(true) {
        s = lightdb->Merge();
    }
    if(!s.ok()){
        s.Report();
    }
}


int main(){
    //test_open();
    //test_str();
    //TestHash_HGet();

    //TestStr_MSet();

    //TestStrMGet();

    //Test_StrExist();

    //TestStrRemove();

    //TestStr_SetNx();
    //TestGetSet();
    //TestStrAppend();

    //TestListLPush();

    //TestListLLen();
    //TestListLPop();
    //TestListLIndex();

    TestMerge();





    return 0;
}