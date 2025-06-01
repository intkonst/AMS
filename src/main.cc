#include <iostream>
#include <thread>
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "handler/handler.h"
#include "api/api.h"

namespace {

    const std::string CONFIG_FILE_NAME = "config.json";
    std::ostringstream out;

}  // namespace

int main(int, char**) {
    if (std::filesystem::exists("log")) {
        std::filesystem::remove_all("log");
    }  // deleting outdated log files

    std::ifstream file(CONFIG_FILE_NAME);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file at " << CONFIG_FILE_NAME << std::endl;
        return 1;
    }

    if (file.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Error: Config file is empty" << std::endl;
        return 1;
    }

    auto config = nlohmann::json::parse(file);

    auto& logger_config = config["main"]["logger"];

    const std::string& LoggerName = logger_config["LOGGER_NAME"];
    const std::string& PathToLoggerFile = logger_config["PATH_TO_LOGGER_FILE"];
    const std::string& LogginLevel = logger_config["LOGGING_LEVEL"];
    const int MaxFileSize = logger_config["MAX_FILE_SIZE"];
    const int MaxFiles = logger_config["MAX_FILES"];

    auto main_logger =
        spdlog::rotating_logger_mt(LoggerName, PathToLoggerFile, MaxFileSize, MaxFiles);

    main_logger->info("run main thread logger");
    out << "run main thread logger with id=" << std::this_thread::get_id();
    main_logger->info(out.str());
    out.clear();

    // create DB >> create object

    std::thread api_thread(api::apiMain);  // run api::api_main function in new api_thread
    std::thread handler_thread(handler::handler_main
    );  // run handler::handler_main function in new handler_thread

    main_logger->info("run api and handler threads");
    api_thread.join();      // main thread wait when api_thread finished his work
    handler_thread.join();  // main thread when handler_thread finished how work
    main_logger->info("stop main with exitkey=0");

    return 0;
}
