#include <iostream>
#include <thread>
#include <chrono>
#include "handler.h"

namespace handler{
void example() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "[handler]: this text write by handler example function" << std::endl;
    return;
}

void handler_main() {
    //code realization is here
    std::cout << "[handler]: handler thread id: "<< std::this_thread::get_id() << std::endl;
    example();
    return;
}
}
