#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <strstream>
#include <thread>

#include <nlohmann/json.hpp>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>


#include "handler.h"
#include "sock/udp_server.h"
#include "../db/db.h"
#include "../api/api.h"

namespace {

    const std::string ConfigFileName = "config.json";
    std::ostrstream out;  // for custom output strings (temporary solution, change to formatting)

    void thread_load_example() { std::this_thread::sleep_for(std::chrono::milliseconds(1000)); }

    std::time_t stringToTstamp(const std::string& datetime) {
        std::tm tm = {};
        std::istringstream ss(datetime);

        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        if (ss.fail()) {
            throw std::runtime_error("Failed to parse datetime string: " + datetime);
        }
        return std::mktime(&tm);
    }

}  // namespace

namespace handler {

    void handler_main() {  // entry point of a stream
        handler::Handler handler {};
        handler.run();
    }

    Handler::Handler() :  Database_("host=localhost dbname=test user=postgres password=1408") {  // init work mode


        std::ifstream file(ConfigFileName);

        if (!file) {
            handler_logger_->error("Error: Could not open config file at {}", ConfigFileName);
            return;
        }

        const auto config = nlohmann::json::parse(file);  // read json with config

        const auto& handler_config = config["handler"];

        PollingRate_ = handler_config["POLLING_RATE"];
        CountOfDevices_ = handler_config["COUNT_OF_DEVICES"];
        CountOfPolls_ = handler_config["COUNT_OF_POLLS"];

        const auto& socket_config = handler_config["socket"];

        SocketConnectionTimeout_ = socket_config["CONNECTION_TIMEOUT"];
        SocketConnectionPort_ = socket_config["CONNECTION_PORT"];

        const auto& logger_config = handler_config["logger"];

        LoggerName_ = logger_config["LOGGER_NAME"];
        PathToLoggerFile_ = logger_config["PATH_TO_LOGGER_FILE"];
        LoggingLevel_ = logger_config["LOGGING_LEVEL"];
        MaxFileSize_ = logger_config["MAX_FILE_SIZE"];
        MaxFiles_ = logger_config["MAX_FILES"];

        // create thread logger

        handler_logger_ =
            spdlog::rotating_logger_mt(LoggerName_, PathToLoggerFile_, MaxFileSize_, MaxFiles_);

        handler_logger_->flush_on(spdlog::level::info);
        spdlog::flush_every(std::chrono::seconds(1));
        handler_logger_->info("run handler thread logger");

        out << "run handler thread with id=" << std::this_thread::get_id();
        handler_logger_->info(out.str());
        out.clear();

        Database_.connect();
        Database_.createTableIfNotExists();


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
            PollingRate_, CountOfDevices_, SocketConnectionTimeout_, SocketConnectionPort_,
            LoggerName_, LoggingLevel_, PathToLoggerFile_, MaxFileSize_, MaxFiles_
        );
    }

    void Handler::run() {  // main work mode
        handler_logger_->info("run main work mode");
        handler_logger_->info("init udp server, wait devices connections...");
        sock::Server udp_server(SocketConnectionPort_, CountOfDevices_, handler_logger_);
        udp_server.showConnectionList();
        handler_logger_->info("init mode finish");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        handler_logger_->info("run device handling");

        while (true) {
            handler_logger_->info("server ping all");

            if (CountOfPolls_ == 0) { break; }

            CountOfPolls_--;

            std::vector<bool> ping_all = udp_server.pingAll();
            for (int indx = 0; indx < ping_all.size(); indx++) {
                handler_logger_->info("#{} is online: {}", indx, std::to_string(ping_all[indx]));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            handler_logger_->info("server get telemetry all");

            std::vector<char*> telemetry_all = udp_server.getTelemetryAll();
            for (int indx = 0; indx < telemetry_all.size(); indx++) {
                handler_logger_->info("#{} telemetry: {}", indx, telemetry_all[indx]);
            }



            db::RecordVector data_to_db = telemetry_all;
            Database_.addRecords(data_to_db);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            handler_logger_->info("Polling iteration end, wait {} millis...", PollingRate_);
            std::this_thread::sleep_for(std::chrono::milliseconds(PollingRate_));

        }


        handler_logger_->info("server exit all");

        std::vector<bool> exit_all = udp_server.exitAll();
        for (int indx=0; indx<exit_all.size(); indx++) {
            handler_logger_->info("#{} exit status: {}", indx,
            std::to_string(exit_all[indx]));
        }

        Database_.printDatabase();

        api::Server server = {Database_, "127.0.0.1"};
        server.run();

    }

    Handler::~Handler() {
        if (handler_logger_) {
            handler_logger_->info("exit handler");
        }
    }

}  // namespace handler
