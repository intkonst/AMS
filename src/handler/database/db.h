#pragma once

#include "record.h"
#include "record_vector.h"
#include <libpq-fe.h>
#include <string>

namespace db {
    class Database {
        PGconn* conn_ = nullptr;
        std::string conninfo_;

      public:
        Database(const std::string& conninfo);
        ~Database();

        bool createTableIfNotExists();
        bool connect();
        void printDatabase();
        bool addRecord(const Record& rec);
        bool checkDuplicates();
        bool removeDuplicates();
        bool ensureUniqueIndex();
        bool addRecords(const RecordVector& records);
        PGresult* executeQuery(const std::string& query);
    };

    // std::time_t string_to_tstamp(const std::string& datetime);

}  // namespace db
