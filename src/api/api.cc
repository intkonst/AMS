#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "api.h"

namespace {
    const std::string ConfigFileName = "config.json";
    std::ostringstream out;
}  // namespace

namespace api {
    void threadLoadExample() { std::this_thread::sleep_for(std::chrono::milliseconds(3000)); }

    void apiMain() {
        /*ЧЕРНОВОЙ ВАРИАНТ ДЛЯ ТЕСТОВ, ПЕРЕДЕЛАТЬ ПОД ПОЛНОЦЕННЫЫЙ КЛАСС*/

        std::ifstream file(ConfigFileName);

        /*if (!file.is_open()) {*/
        if (!file) {
            std::cerr << "Error: Could not open config file at " << ConfigFileName << std::endl;
            return;
        }

        if (file.peek() == std::ifstream::traits_type::eof()) {
            std::cerr << "Error: Config file is empty" << std::endl;
            return;
        }

        auto config = nlohmann::json::parse(file);

        auto& logger_config = config["api"]["logger"];

        const std::string& LoggerName = logger_config["LOGGER_NAME"];
        const std::string& PathToLoggerFile = logger_config["PATH_TO_LOGGER_FILE"];
        const std::string& LoggingLevel = logger_config["LOGGING_LEVEL"];
        int MaxFileSize = logger_config["MAX_FILE_SIZE"];
        int MaxFiles = logger_config["MAX_FILES"];

        auto api_logger =
            spdlog::rotating_logger_mt(LoggerName, PathToLoggerFile, MaxFileSize, MaxFiles);

        api_logger->info("run api thread logger");

        out << "run api thread with id=" << std::this_thread::get_id();
        api_logger->info(out.str());
        api_logger->info("run api thread with id=%T");
        out.clear();
        api_logger->info("run load test - wait 3 sec");
        threadLoadExample();
        api_logger->info("load test completed");
    }
}  // namespace api
