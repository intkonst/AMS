#include <iostream>
#include <thread>
#include <chrono>

#include "api.h"


namespace api {

void thread_load_example() {
    std::cout << "[api]: load test - wait 3 sec" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    std::cout << "[api]: load test completed" << std::endl;
    return;
}

void api_main() {
    std::cout << "[api]: run api thread with id: "<< std::this_thread::get_id() << std::endl;
    thread_load_example();
    return;
}

}
