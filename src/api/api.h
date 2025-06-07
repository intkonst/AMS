#pragma once

#include <string>

#include <httplib.h>
#include <spdlog/logger.h>

#include "../db/db.h"


namespace api {

    void apiMain(db::Database* database);

    class API { //TODO: add logging
      private:
        httplib::Server server_;
        db::Database& database_;
        std::string host_;
        int port_;

        std::shared_ptr<spdlog::logger> api_logger_;
        std::string LoggerName_;
        std::string PathToLoggerFile_;
        std::string LoggingLevel_;
        int MaxFileSize_;
        int MaxFiles_;
        
      public:
        API(db::Database* database, std::string host="0.0.0.0", int port=8080);
        void run();
        ~API();
    };

}  // namespace api
