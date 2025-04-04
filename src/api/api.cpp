#include <iostream>
#include <thread>
#include <chrono>
#include "api.h"

namespace api {
void example() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "[api]: this text write by api example function" << std::endl;
    return;
}

void api_main() {
    //code realization is here
    std::cout << "[api]: api thread id: "<< std::this_thread::get_id() << std::endl;
    example();
    return;
}
}
