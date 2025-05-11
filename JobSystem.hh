#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include "Queue.hh"

struct Job {
    std::function<void()> f;
};

typedef Queue<Job> concurrent_queue;
class JobSystem {
public:
    JobSystem() = default;
    ~JobSystem() = default;

    void start(int n) {
        isRunning.store(true);
        for (int i = 0; i < n; i++) {
            threads.emplace_back([this]() {
                this->run();
            });
        }
    }

    void run() {
        while (isRunning.load()) {
            std::optional<Job> j = concurrent_queue::instance()->pop();
            if (not j.has_value()) {
                continue;
            }
            j->f();
        }
    }

    void stop() {
        concurrent_queue::instance()->cancel();
        isRunning.store(false);
        for (auto &t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }      
    }

private:
    std::vector<std::thread> threads;
    std::atomic<bool> isRunning{false};
};