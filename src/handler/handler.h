#pragma once

#include <string>

#include <spdlog/logger.h>

namespace handler {

    void handler_main();

    class Handler {
      private:
        int PollingRate_;
        int CountOfDevices_;
        int SocketConnectionTimeout_;
        int CountOfPolls_;
        int SocketConnectionPort_;

        std::string LoggerName_;
        std::string PathToLoggerFile_;
        std::string LoggingLevel_;
        int MaxFileSize_;
        int MaxFiles_;

        std::shared_ptr<spdlog::logger> handler_logger_;

      public:
        Handler();
        void run();
        ~Handler();
    };

}  // namespace handler
