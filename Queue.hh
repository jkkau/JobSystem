#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <atomic>

template<typename T>
class Queue {
public:
    // 要支持完美转发需要该函数是模板函数，不能直接使用类模板T
    template<typename U>
    void push(U&& t) {
        {
            std::lock_guard lk(mtx);
            q.emplace_back(std::forward<U>(t));
        }
        cv.notify_one();
    }
    std::optional<T> pop(int timeoutMs) {
        std::unique_lock lk(mtx);
        while (!stop && q.empty()) {
            auto status = cv.wait_for(lk, std::chrono::milliseconds(timeoutMs));
            if (status == std::cv_status::timeout) {
                return std::nullopt;
            }
        }
        if (stop) {
            return std::nullopt;
        }

        T t = q.front();
        q.pop_front();
        return std::optional<T>(t);
    }
    void cancel() {
        stop.store(true);
        cv.notify_all();
    }
    static Queue<T>* instance() {
        static Queue<T> q;
        return &q;
    }

private:
    std::deque<T> q;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stop{false};
};
