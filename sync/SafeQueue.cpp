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
        size--;
        return true;
    }

    template<class E>
    void SafeQueue<E>::Enqueue(E& e) {
        std::unique_lock<std::mutex> lk(mtx);
        queue.push(e);
        size++;
        cv.notify_all();
    }

    template<class E>
    bool SafeQueue<E>::Empty() {
        std::unique_lock<std::mutex> lk(mtx);
        bool res = size == 0;
        return res;
    }

}