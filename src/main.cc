#include <iostream>
#include <thread>
#include <filesystem>
#include <fstream>
#include <fmt/core.h>


#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "db/db.h"
#include "handler/handler.h"
#include "api/api.h"


namespace {
    const std::string ConfigFilePath = "config.json";
}  // namespace

int main(int, char**) {

    if (std::filesystem::exists("log")) { // init main -> read config for main thread
        std::filesystem::remove_all("log");
    }  // deleting outdated log files

    std::ifstream file(ConfigFilePath);

    if (!file) {  // check file exist
        std::cerr << "Error: Could not open config file at " << ConfigFilePath << std::endl;
        return 1;
    }


    auto config = nlohmann::json::parse(file);

    auto& logger_config = config["main"]["logger"];

    const std::string& LoggerName = logger_config["LOGGER_NAME"];
    const std::string& PathToLoggerFile = logger_config["PATH_TO_LOGGER_FILE"];
    const std::string& LogginLevel = logger_config["LOGGING_LEVEL"];
    const int MaxFileSize = logger_config["MAX_FILE_SIZE"];
    const int MaxFiles = logger_config["MAX_FILES"];

    auto main_logger =  // run main thread logger logger
    
    spdlog::rotating_logger_mt(LoggerName, PathToLoggerFile, MaxFileSize, MaxFiles); 
    
    main_logger->flush_on(spdlog::level::info);
    spdlog::flush_every(std::chrono::seconds(1));

    main_logger->info(fmt::format("run main thread with id={}", std::this_thread::get_id()));

    // create DB (add init here)
    db::Database database("host=localhost dbname=test user=postgres password=1408");
    database.connect();
    database.createTableIfNotExists();
    
    std::thread api_thread(api::apiMain, database); //init new threads
    std::thread handler_thread(handler::handler_main, database); 

    main_logger->info("run api and handler threads");
    api_thread.join();
    handler_thread.join();  
    main_logger->info("stop main thread");

    return 0;
}
