//
// Created by xiaolei on 2022/3/20.
//

#ifndef MYPROJECT_SAFEQUEUE_H
#define MYPROJECT_SAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
namespace lightdb{

template<class E>
class SafeQueue{
private:
    std::queue<E> queue;
    std::condition_variable cv;
    std::mutex mtx;
    int size = 0;

public:
    void Enqueue(const E& e){
        std::unique_lock<std::mutex> lk(mtx);
        queue.push(e);
        size++;
        cv.notify_all();
    }

    bool Dequeue(E& e){
        std::unique_lock<std::mutex> lk(mtx);
        while(size <= 0) {
            if (size <= 0) {
                cv.wait(lk);
            }
        }
        e = queue.front();
        queue.pop();
        size--;
        return true;  
    }

    bool Empty(){
        // 因为用到了size，可能会和Enqueue或者Dequeue竞争，所以要加锁
        std::unique_lock<std::mutex> lk(mtx);
        bool res = size == 0;
        return res;
    }

    int Size(){
        std::unique_lock<std::mutex> lk(mtx);
        return queue.size();
    }
};
}
#endif //MYPROJECT_SAFEQUEUE_H
