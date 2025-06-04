#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <strstream>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "handler.h"
#include "sock/udp_server.h"

namespace {

    const std::string ConfigFileName = "config.json";
    std::ostrstream out;  // for custom output strings (temporary solution, change to formatting)

    void thread_load_example() { std::this_thread::sleep_for(std::chrono::milliseconds(1000)); }

}  // namespace

namespace handler {

    void handler_main() {  // entry point of a stream
        handler::Handler handler {};
        handler.run();
    }

    Handler::Handler() {  // init work mode

        std::ifstream file(ConfigFileName);

        if (!file) {
            handler_logger_->error("Error: Could not open config file at {}", ConfigFileName);
            return;
        }

        const auto config = nlohmann::json::parse(file);  // read json with config

        PollingRate_ = config["handler"]["POLLING_RATE"];
        CountOfDevices_ = config["handler"]["COUNT_OF_DEVICES"];

        const auto& socket_config = config["handler"]["socket"];

        SocketConnectionTimeout_ = socket_config["CONNECTION_TIMEOUT"];
        SocketConnectionPort_ = socket_config["CONNECTION_PORT"];


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

        // output config data to logger for check

        handler_logger_->debug(
            "read config file\n"
            "POLLING_RATE: {}\n"
            "COUNT_OF_DEVICES: {}\n"
            "CONNECTION_TIMEOUT: {}\n"
            "CONNECTION_PORT: {}\n"
            "LOGGER_NAME: {}\n"
            "LOGGING_LEVEL: {}\n"
            "PATH_TO_LOGGER_FILE: {}\n"
            "MAX_FILE_SIZE: {}\n"
            "MAX_FILES: {}",
            PollingRate_, CountOfDevices_, SocketConnectionTimeout_, SocketConnectionPort_ , LoggerName_, LoggingLevel_, PathToLoggerFile_,
            MaxFileSize_, MaxFiles_
        );
    }

    void Handler::run() {  // main work mode
        handler_logger_->info("run main work mode");
        handler_logger_->info("init udp server, wait devices connections...");
        sock::Server udp_server(SocketConnectionPort_, CountOfDevices_, handler_logger_);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        udp_server.showConnectionList();

        handler_logger_->info("run device handling");
    }

    Handler::~Handler() {
        if (handler_logger_) {
            handler_logger_->info("exit handler");
        }
    }

}  // namespace handler
