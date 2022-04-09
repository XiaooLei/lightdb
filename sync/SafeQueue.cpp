//
// Created by 9 on 2022/3/20.
//

#include "SafeQueue.h"

namespace lightdb{
    template<class E>
    bool SafeQueue<E>::Dequeue(E& e) {
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

    template<class E>
    void SafeQueue<E>::Enqueue(const E& e) {
        std::unique_lock<std::mutex> lk(mtx);
        queue.push(e);
        size++;
        cv.notify_all();
    }

    template<class E>
    bool SafeQueue<E>::Empty() {
        // 因为用到了size，可能会和Enqueue或者Dequeue竞争，所以要加锁
        std::unique_lock<std::mutex> lk(mtx);
        bool res = size == 0;
        return res;
    }

}
