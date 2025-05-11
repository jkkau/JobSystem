#include "JobSystem.hh"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main() {
    JobSystem js(2);
    js.start();

    for (int i = 0; i < 10000; i++) {
        js.submit(Job{
            f : [i]() {std::cout << "hello again, i = " << i << ", "<< std::this_thread::get_id() << std::endl; }
        });
    }


    std::this_thread::sleep_for(1000ms);

    js.stop();
}