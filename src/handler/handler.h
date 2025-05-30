#pragma once


#include <string>

#include <spdlog/logger.h>
#include "spdlog/sinks/rotating_file_sink.h"


namespace {

void thread_load_example();

}


namespace handler {

void handler_main();

class Handler {
    private:
        //NET CONFIG
        int NET_POLLING_RATE_;
        int NET_CLIENT_CONNECTION_TIMEOUT_;

        //LOGGING CONFIG
        std::string LOGGER_NAME_;
        std::string PATH_TO_LOGGER_FILE_;
        std::string LOGGING_LEVEL_;
        int MAX_FILE_SIZE_;
        int MAX_FILES_;
        std::shared_ptr<spdlog::logger> handler_logger_;

    public:
        Handler();
        void run();
        ~Handler();
};

}
