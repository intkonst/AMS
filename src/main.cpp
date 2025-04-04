#include <iostream>
#include <thread>
#include <chrono>
#include "./handler/handler.h"
#include "./api/api.h"

int main (int, char**) {
    std::cout << "[main]: main thread id: " <<  std::this_thread::get_id() << std::endl;
    std::thread api_thread(api::api_main); //run api::api_main function in new api_thread
    std::thread handler_thread(handler::handler_main); // run handler::handler_main function in new handler_thread

    // main thread code write here

    api_thread.join(); //main thread wait when api_thread finished his work
    handler_thread.join(); //main thread when handler_thread finished how work

    //run destructors
    
    return 0;
}
