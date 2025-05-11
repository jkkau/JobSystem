#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include "Queue.hh"
#include "AtomicRingQueue.hh"

struct Job {
    std::function<void()> f;
};

struct ThreadContext {
    AtomicRingQueue<Job> q;
    int id;
};

typedef Queue<Job> global_queue;
class JobSystem {
public:
    JobSystem(int n) : threadNumber(n),threadCtxs(n)  {}
    ~JobSystem() = default;

    void start() {
        isRunning.store(true);

        for (int i = 0; i < threadNumber; i++) {
            threads.emplace_back([this, i]() {
                threadCtxs[i].id = i;
                localCtx = &threadCtxs[i];
                this->run();
            });
        }
    }

    void run() {
        while (isRunning.load()) {
            std::optional<Job> j;
            if (localCtx) {
                j = localCtx->q.pop();
                if (j) {
                    j->f();
                    continue;
                }
            }

            // 等10ms等不到pop就返回，避免阻塞在global queue上造成local queue饥饿
            j = global_queue::instance()->pop(10 /*ms*/);
            if (j) {
                j->f();
            }
        }
    }

    void stop() {
        global_queue::instance()->cancel();
        isRunning.store(false);
        for (auto &t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }      
    }

    void submit(Job j) {
        // 使用round robin的方式来选则thread queue
        static int picker = 0;
        picker = (picker+1)%threadNumber;
        ThreadContext *tc = &threadCtxs[picker];
        // 先push到thread local queue, 如果local queue满了则push到global queue
        if (tc && tc->q.push(j)) {
            return;
        }
        global_queue::instance()->push(j);
    }

private:
    int threadNumber;
    std::vector<std::thread> threads;
    std::vector<ThreadContext> threadCtxs;
    static thread_local ThreadContext *localCtx;
    std::atomic<bool> isRunning{false};
};

thread_local ThreadContext *JobSystem::localCtx = nullptr;