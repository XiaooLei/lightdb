//
// Created by 9 on 2022/3/20.
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
    void Enqueue(const E& e);

    bool Dequeue(E& e);

    bool Empty();
};
}
#endif //MYPROJECT_SAFEQUEUE_H
