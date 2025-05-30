#include <iostream>
#include <thread>

#include "handler.h"


namespace {

void thread_load_example() {
    std::cout << "[handler]: load test - wait 1 sec" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "[handler]: load test completed" << std::endl;
    return;
}

} // namespace


namespace handler {

void handler_main() {
    auto handler = handler::Handler();
    handler.run();
    handler.~Handler();
};
    

Handler::Handler() {
    std::cout << "[handler]: run handler thread with id: "<< std::this_thread::get_id() << std::endl;
    //read config
    //init net
};

void Handler::run() {
    std::cout << "[handler]: run main mode" << std::endl;
    thread_load_example();
    // while
};

Handler::~Handler() {
    std::cout << "[handler]: exit handler" << std::endl;
};

} // namespace handler
