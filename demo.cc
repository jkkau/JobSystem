#include "JobSystem.hh"
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

int main() {
    JobSystem js;
    js.start(2);

    Job j;
    j.f = []() {
        std::cout << "hello job\n";
    };
    concurrent_queue::instance()->push(j);
    concurrent_queue::instance()->push(Job{
        f : []() {std::cout << "hello again\n"; }
    });

    std::this_thread::sleep_for(1000ms);

    js.stop();
}