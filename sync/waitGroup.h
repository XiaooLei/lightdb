//
// Created by xiaolei on 2022/2/27.
//

#ifndef MYPROJECT_WAITGROUP_H
#define MYPROJECT_WAITGROUP_H

#include <atomic>
#include <condition_variable>

class WaitGroup{
public:
    WaitGroup(){
        counter.store(0);
    }

    void Add(int num){
        counter += num;
    }

    void Done(){
        counter--;
        if(counter <=0 ){
            cond.notify_all();
        }
    }

    void Wait(){
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, [&] { return counter <= 0; });
    }
private:
    std::mutex mutex;
    std::atomic<int> counter;
    std::condition_variable cond;

};

#endif //MYPROJECT_WAITGROUP_H
