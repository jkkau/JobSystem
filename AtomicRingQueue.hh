#pragma once

#include <atomic>
#include <optional>

// 保证单线程写, 单线程读是线程安全的
template<typename T, int N = 1024>
class AtomicRingQueue {
public:
    AtomicRingQueue() = default;
    ~AtomicRingQueue() = default;

    bool push(T ele) {
        int h = head.load();
        int t = tail.load();

        int next = (t+1)%N;
        if (next == h) {
            // 队列已满
            // 实际上当前的t还可以存储数据，使用空一格的方式来判断队列满
            return false;
        }
        q[t] = ele;
        tail.store(next);
        return true;
    }
    std::optional<T> pop() {
        int h = head.load();
        int t = tail.load();
        if (h == t) {
            // 队列是空的
            return std::nullopt;
        }
        T result = q[h];
        head.store((h+1)%N);
        return std::optional<T>(result);
    }

private:
    T q[N];
    std::atomic<int> head{0};
    std::atomic<int> tail{0};
};