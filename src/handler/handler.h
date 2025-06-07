#pragma once

#include "../db/db.h"

#include <string>

#include <spdlog/logger.h>

namespace handler {

    void handler_main(db::Database);

    class Handler {
      private:
        int PollingRate_;
        int CountOfDevices_;
        int SocketConnectionTimeout_;
        int CountOfPolls_;
        int SocketConnectionPort_;

        db::Database database_;

        std::shared_ptr<spdlog::logger> handler_logger_;
        std::string LoggerName_;
        std::string PathToLoggerFile_;
        std::string LoggingLevel_;
        int MaxFileSize_;
        int MaxFiles_;

        

      public:
        Handler(db::Database database);
        void run();
        ~Handler();
    };

}  // namespace handler
