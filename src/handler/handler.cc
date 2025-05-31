#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "handler.h"

namespace {

    const std::string ConfigFileName = "config.json";
    std::ostringstream out;  // for custom output strings

    void thread_load_example() { std::this_thread::sleep_for(std::chrono::milliseconds(1000)); }

}  // namespace

namespace handler {

    void handler_main() {  // entry point of a stream
        handler::Handler handler {};
        handler.run();
    }

    Handler::Handler() {  // init work mode

        // read config file

        std::ifstream file(ConfigFileName);

        if (!file) {
            std::cerr << "Error: Could not open config file at " << ConfigFileName << std::endl;
            return;
        }

        if (file.peek() == std::ifstream::traits_type::eof()) {
            std::cerr << "Error: Config file is empty" << std::endl;
            return;
        }

        const auto config = nlohmann::json::parse(file);

        NetPollingRate_ = config["handler"]["NET_POLLING_RATE"];
        NetClientConnectionTimeout_ = config["handler"]["NET_CLIENT_CONNECTION_TIMEOUT"];

        const auto& logger_config = config["handler"]["logger"];

        LoggerName_ = logger_config["LOGGER_NAME"];
        PathToLoggerFile_ = logger_config["PATH_TO_LOGGER_FILE"];
        LoggingLevel_ = logger_config["LOGGING_LEVEL"];
        MaxFileSize_ = logger_config["MAX_FILE_SIZE"];
        MaxFiles_ = logger_config["MAX_FILES"];

        // create thread logger

        handler_logger_ =
            spdlog::rotating_logger_mt(LoggerName_, PathToLoggerFile_, MaxFileSize_, MaxFiles_);

        handler_logger_->info("run handler thread logger");
        out << "run handler thread with id=" << std::this_thread::get_id();
        handler_logger_->info(out.str());
        out.clear();

        // check config data

        handler_logger_->info(
            "read config file\n"
            "NET_POLLING_RATE: {}\n"
            "NET_CLIENT_CONNECTION_TIMEOUT: {}\n"
            "LOGGER_NAME: {}\n"
            "LOGGING_LEVEL: {}\n"
            "PATH_TO_LOGGER_FILE: {}\n"
            "MAX_FILE_SIZE: {}\n"
            "MAX_FILES: {}",
            NetPollingRate_, NetClientConnectionTimeout_, LoggerName_, LoggingLevel_,
            PathToLoggerFile_, MaxFileSize_, MaxFiles_
        );
    }

    void Handler::run() {  // main work mode
        handler_logger_->info("run main work mode");
        handler_logger_->info("run load test - wait 1 sec");
        thread_load_example();
        handler_logger_->info("load test completed");
    }

    Handler::~Handler() {
        if (handler_logger_) {
            handler_logger_->info("exit handler");
        }
     }

}  // namespace handler
