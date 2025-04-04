#include <iostream>
#include <thread>
#include <chrono>

//#include <yamlparser.h>

#include "./handler/handler.h"
#include "./api/api.h"



int main (int, char**) {
    std::cout << "[main]: main thread id: " <<  std::this_thread::get_id() << std::endl;
    std::thread api_thread(api::api_main);
    std::thread handler_thread(handler::handler_main);
    api_thread.join();
    handler_thread.join();
    return 0;
}
