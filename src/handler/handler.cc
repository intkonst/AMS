#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "handler.h"


namespace {

const std::string CONFIG_FILE_NAME = "config.json";
std::ostringstream out; // for custom output strings 

void thread_load_example() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

} // namespace


namespace handler {

void handler_main() { // entry point of a stream
    handler::Handler handler{};
    handler.run();
};

Handler::Handler(){ // init work mode
    
    //read config file

    std::ifstream file(CONFIG_FILE_NAME);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file at " << CONFIG_FILE_NAME << std::endl;
        return;
    }
    
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

    //create thread logger

    handler_logger_ = spdlog::rotating_logger_mt(LOGGER_NAME_, PATH_TO_LOGGER_FILE_,\
         MAX_FILE_SIZE_, MAX_FILES_);
    
    handler_logger_->info("run handler thread logger");
    out << "run handler thread with id=" << std::this_thread::get_id();
    handler_logger_->info(out.str());
    out.clear();

    // check config data  

    out << "read config file" << std::endl
        << "NET_POLLING_RATE: " << NET_POLLING_RATE_ << std::endl
        << "NET_CLIENT_CONNECTION_TIMEOUT: " << NET_CLIENT_CONNECTION_TIMEOUT_ << std::endl
        << "LOGGER_NAME: " << LOGGER_NAME_ << std::endl
        << "LOGGING_LEVEL: " << LOGGING_LEVEL_ << std::endl
        << "PATH_TO_LOGGER_FILE: " << PATH_TO_LOGGER_FILE_ << std::endl
        << "MAX_FILE_SIZE: " << MAX_FILE_SIZE_ << std::endl
        << "MAX_FILES: " << MAX_FILES_;
    
    handler_logger_->info(out.str()); 
    out.clear();
};

void Handler::run() { // main work mode
    handler_logger_->info("run main work mode");
    handler_logger_->info("run load test - wait 1 sec");
    thread_load_example();
    handler_logger_->info("load test completed");
};

Handler::~Handler() {
    handler_logger_->info("exit handler");
};

} // namespace handler
