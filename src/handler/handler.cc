#include <iostream>
#include <fstream>
#include <nlohmann/json_fwd.hpp>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "handler.h"


namespace {

const std::string PATH_TO_CONFIG_FILE = "config.json"; 

void thread_load_example() {
    std::cout << "[handler]: load test - wait 1 sec" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "[handler]: load test completed" << std::endl;
    return;
}


} // namespace


namespace handler {

void handler_main() {
    handler::Handler handler{};
    handler.run();
};

Handler::Handler(){

    std::cout << "[handler]: run handler thread with id: "<< std::this_thread::get_id() << std::endl;

    // read data from config.json

    std::ifstream file(PATH_TO_CONFIG_FILE);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file at " << PATH_TO_CONFIG_FILE << std::endl;
        return; // или обработка ошибки
    }
    
    // Проверка, что файл не пустой
    if (file.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Error: Config file is empty" << std::endl;
        return;
    }


    auto config = nlohmann::json::parse(file);

    NET_POLLING_RATE_ = config["handler"]["NET_POLLING_RATE"];
    NET_CLIENT_CONNECTION_TIMEOUT_ = config["handler"]["NET_CLIENT_CONNECTION_TIMEOUT"];
        
    auto& logger_config = config["handler"]["logger"];
        
    LOGGER_NAME_ = logger_config["LOGGER_NAME"];
    PATH_TO_LOGGER_FILE_ = logger_config["PATH_TO_LOGGER_FILE"];
    LOGGING_LEVEL_ = logger_config["LOGGING_LEVEL"];
    MAX_FILE_SIZE_ = logger_config["MAX_FILE_SIZE"];
    MAX_FILES_ = logger_config["MAX_FILES"];

    // check

    std::cout << NET_POLLING_RATE_ << std::endl;
    std::cout << NET_CLIENT_CONNECTION_TIMEOUT_ << std::endl;
    std::cout << LOGGER_NAME_ << std::endl;
    std::cout << LOGGING_LEVEL_ << std::endl;
    std::cout << PATH_TO_LOGGER_FILE_ << std::endl;
    std::cout << MAX_FILE_SIZE_ << std::endl;
    std::cout << MAX_FILES_ << std::endl;

};

void Handler::run() {
    std::cout << "[handler]: run main mode" << std::endl;
    thread_load_example();
};

Handler::~Handler() {
    std::cout << "[handler]: exit handler" << std::endl;
};

} // namespace handler
