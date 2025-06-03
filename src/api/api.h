#pragma once

#include <httplib.h>

#include "../db/db.h"


namespace api {

    void apiMain();
    void example();

    class Server { //TODO: add logging
      private:
        db::Database& database;
        std::string host;
        int port;
        httplib::Server server;
      public:
        Server(db::Database database, std::string host="0.0.0.0", int port=8080);

        void run();

        ~Server();
    };

}  // namespace api
