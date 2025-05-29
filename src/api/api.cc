#include <iostream>
#include <thread>
#include <chrono>
#include "api.h"

namespace api {
void example() {
    std::cout << "[api]: wait 1 sec." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "[api]: this text write by api example function" << std::endl;
    return;
}

void api_main() {
    std::cout << "[api]: api thread id: "<< std::this_thread::get_id() << std::endl;
    example();
    return;
}
}
