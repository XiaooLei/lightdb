

#include <iostream>
#include <vector>
#include "ds_zset.h"
#include <cassert>
using namespace lightdb;

int test_zincrby(){

    Status s;
    SortedSet zset;

    //test ZIncrBy
    zset.ZAdd("test_zincrby",1,"lebron");
    zset.ZAdd("test_zincrby",2,"kobe");
    zset.ZAdd("test_zincrby",3,"durant");
    int rank;
    int ret;
    s = zset.ZRem("test_zincrby","kobe");
    //cout<<"ret "<<ret<<endl;
    s = zset.ZRank("test_zincrby","durant",rank);
    assert(rank == 1);
    assert(zset.ZCard("test_zincrby") == 2);
    s = zset.ZRank("test_zincrby", "kobe", rank);
    assert(s.ok());
    printf("******** ZAdd\n");
    s = zset.ZAdd("test_zincrby", 5, "kobe");
    assert(s.ok());

    printf("******* ZRank\n");
    s = zset.ZRank("test_zincrby", "kobe", rank);
    assert(s.ok());

    assert(rank == 2);

    s = zset.ZIncrBy("test_zincrby", 10,"lebron");
    assert(s.ok());
    s = zset.ZRank("test_zincrby","lebron", rank);
    assert(s.ok());
    assert(rank == 2);
    s = zset.ZRank("test_zincrby","durant",rank);
    assert(s.ok());
    assert(rank == 0);

    s = zset.ZRank("test_zincrby","kobe",rank);
    assert(s.ok());
    assert(rank == 1);

    s = zset.ZAdd("test_zincrby", 7, "paul");
    assert(s.ok());
    s = zset.ZRank("test_zincrby","lebron",rank);
    assert(s.ok());
    assert(rank == 3);   
    
    s = zset.ZRank("test_zincrby","paul",rank);
    assert(s.ok());
    assert(rank == 2);  


    //test again

    zset.ZAdd("ttest",0,"key0");
    zset.ZAdd("ttest",1,"key1");
    zset.ZAdd("ttest",2,"key2");
    zset.ZAdd("ttest",3,"key3");
    zset.ZAdd("ttest",4,"key4");
    zset.ZAdd("ttest",5,"key5");
    zset.ZAdd("ttest",6,"key6");
    zset.ZAdd("ttest",7,"key7");
    zset.ZAdd("ttest",8,"key8");
    cout<<"****************"<<endl;
    s = zset.ZRank("ttest","key5",rank);
    assert(s.ok());
    cout<<"rank :"<< rank <<endl;
    assert(rank == 5);
    zset.ZIncrBy("ttest",100,"key3");
    s = zset.ZRank("ttest","key3",rank);
    assert( s.ok() );
    assert(rank==8);
    s = zset.ZRank("ttest","key5",rank);
    assert( s.ok() );
    assert(rank == 4);
}


int test_zrange(){
    SortedSet zset;
    zset.ZAdd("ttest", 1, "aaa");
    zset.ZAdd("ttest", 2, "bbb");
    zset.ZAdd("ttest", 3, "ccc");
    zset.ZAdd("ttest", 4, "ddd");
    std::vector<std::string> vals;
    zset.ZRange("ttest", 2, 3, vals);
    for(auto it=vals.begin(); it!=vals.end(); it++){
        cout<<*it<<endl;
    }
    return 0;
}


int test(){
        //test zset ZAdd
    Status s;
    SortedSet zset;
    assert(zset.ZCard("test_zadd") == 0);
    zset.ZAdd("test_zadd",1,"guangzhou");
    zset.ZAdd("test_zadd",2,"changsha");
    zset.ZAdd("test_zadd",3,"hangzhou");
    assert(zset.ZCard("test_zadd") == 3);
    cout<<"valid test zset zadd!"<<endl;

    //test zset ZRem,ZScore
    zset.ZAdd("test_zrem", 1, "tencent");
    zset.ZAdd("test_zrem", 2, "alibaba");
    zset.ZAdd("test_zrem", 3, "bytedance");
    double score;
    s = zset.ZScore("test_zrem","tencent",score);
    if( !s.ok() ){
        cout<<"ZScore failed"<<endl;
    }
    assert(score == 1.0);
    zset.ZRem("test_zrem", "bytedance");
    int new_card = zset.ZCard("test_zrem");
    cout<< new_card <<endl;
    assert(new_card == 2);
    cout<<"valid test zset zscore !"<<endl;

    //test zrank
    zset.ZAdd("test_zrank",1,"taobao");
    zset.ZAdd("test_zrank",2,"wechat");
    zset.ZAdd("test_zrank",2,"tiktok");
    zset.ZAdd("test_zrank",4,"google");
    int rank;
    zset.ZRank("test_zrank","tiktok",rank);
    //cout<<rank<<endl;
    assert(rank == 1);
    zset.ZRank("test_zrank","taobao",rank);
    assert(rank == 0);
    zset.ZRank("test_zincrby","kobe",rank);
    assert(rank == 1);
    s = zset.ZRem("test_zincrby","kobe");
    if( !s.ok() ){
        printf("zrem faild,code %d \n", s.Code());
    }
    zset.ZRank("test_zincrby","durant",rank);
    assert(rank == 1);
    zset.ZRank("test_zincrby","lebron",rank);
    assert(rank == 0);
    zset.ZAdd("test_zincrby",2,"kobe");
    
    s = zset.ZIncrBy("test_zincrby", 3, "kobe");
    if( !s.ok() ){
        printf("zset zincrby failed code:%d  \n",s.Code());
        return 0;
    }
    zset.ZRank("test_zincrby","kobe",rank);
    cout<<"zincryby rank:"<<rank<<endl;
    assert(rank == 2);
    cout<< "valid test zincrby !" <<endl;

    //debug;
}

int main(){
    //test_zincrby();
    //test();
    test_zrange();
    return 0;

}