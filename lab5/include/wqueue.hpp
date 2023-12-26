#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace lab5 {

template<class T>
class TWQueue {
private:

    std::queue<T> q;
    std::mutex mtx;
    std::condition_variable cv;

public:

    void push(const T& el) {
        {
            std::unique_lock l(mtx);
            q.push(el);
        }
        cv.notify_one();
    }

    T pop() {
        std::unique_lock l(mtx);
        if (q.empty()) {
            cv.wait(l);
        }
        T result = q.front();
        q.pop();
        return result;
    }

    size_t size() {
        std::unique_lock l(mtx);
        return q.size();
    }

};

}
