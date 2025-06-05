#include <iostream>
#include <iomanip>

#include "db.h"

namespace db {
    Database::Database(const std::string& conninfo) : conninfo_(conninfo) {}

    Database::~Database() {
        if (conn_) {
            PQfinish(conn_);
        }
    }

    bool Database::connect() {
        std::cout << "Connecting with conninfo: " << conninfo_ << std::endl;

        if (!conn_) {
            std::cerr << "[Database] Already connected" << std::endl;
            return false;
        }

        conn_ = PQconnectdb(conninfo_.c_str());

        if (conn_ == nullptr) {
            std::cerr << "[Database] PQconnectdb returned nullptr" << std::endl;
            return false;
        }
        if (PQstatus(conn_) != CONNECTION_OK) {
            std::cerr << "[Database] Connection failed: " << PQerrorMessage(conn_) << std::endl;
            PQfinish(conn_);
            conn_ = nullptr;
            return false;
        }

        std::cout << "[Database] Connected successfully.\n";
        return true;
    }

    bool Database::createTableIfNotExists() {
        const char* sql =
            "CREATE TABLE IF NOT EXISTS measurements ("
            "id SERIAL PRIMARY KEY, "
            "tstamp TIMESTAMP WITHOUT TIME ZONE, "
            "device_id TEXT, "
            "temperature REAL, "
            "humidity REAL, "
            "brightness REAL, "
            "test BOOLEAN, "
            "UNIQUE(tstamp, device_id)"
            ");";

        PGresult* res = PQexec(conn_, sql);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Create table failed: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    }

    bool Database::checkDuplicates() {
        const char* sql = R"(
        SELECT tstamp, device_id, COUNT(*)
        FROM measurements
        GROUP BY tstamp, device_id
        HAVING COUNT(*) > 1
        LIMIT 1;
    )";

        PGresult* res = PQexec(conn_, sql);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::cerr << "Failed to check duplicates: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res);
            return false;  // считать что дубликатов нет или ошибка
        }

        int nrows = PQntuples(res);
        PQclear(res);
        return nrows > 0;
    }

    bool Database::removeDuplicates() {
        const char* sql = R"(
        DELETE FROM measurements a
        USING measurements b
        WHERE
            a.id > b.id
            AND a.tstamp = b.tstamp
            AND a.device_id = b.device_id;
    )";

        PGresult* res = PQexec(conn_, sql);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Failed to remove duplicates: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    }

    bool Database::ensureUniqueIndex() {
        const char* sql = R"(
        DO $$
        BEGIN
            IF NOT EXISTS (
                SELECT 1 FROM pg_constraint
                WHERE conname = 'unique_tstamp_device_id'
            ) THEN
                ALTER TABLE measurements ADD CONSTRAINT unique_tstamp_device_id UNIQUE (tstamp, device_id);
            END IF;
        END
        $$;
    )";

        PGresult* res = PQexec(conn_, sql);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Failed to add unique constraint: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    }

    void Database::printDatabase() {
        const char* query =
            "SELECT id, tstamp, device_id, temperature, humidity, "
            "brightness, test FROM measurements;";
        PGresult* res = PQexec(conn_, query);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::cerr << "SELECT failed: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res);
            return;
        }

        int nrows = PQntuples(res);
        int nfields = PQnfields(res);

        const int col_width = 20;

        for (int i = 0; i < nfields; i++) {
            std::cout << std::left << std::setw(col_width) << PQfname(res, i);
        }

        std::cout << "\n";
        std::cout << std::string(nfields * col_width, '-') << "\n";

        for (int i = 0; i < nrows; i++) {
            for (int j = 0; j < nfields; j++) {
                std::cout << std::left << std::setw(col_width) << PQgetvalue(res, i, j);
            }
            std::cout << "\n";
        }

        PQclear(res);
    }

    bool Database::addRecord(const Record& rec) {
        std::string query = rec.insertQuery();

        PGresult* res = PQexec(conn_, query.c_str());

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "[Database] Insert failed: " << PQerrorMessage(conn_) << std::endl;
            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool Database::addRecords(const RecordVector& records) {
        bool all_success = true;

        for (const auto& rec : records) {
            if (!addRecord(rec)) {
                std::cerr << "[Database] Failed to insert record." << ")\n";
                all_success = false;
            }
        }

        return all_success;
    }

    bool Database::executeQuery(const std::string& query) {
        if (!conn_) {
            std::cerr << "[Database] No connection to execute query.\n";
            return false;
        }

        PGresult* res = PQexec(conn_, query.c_str());

        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::cerr << "[Database] Query execution failed: " << PQerrorMessage(conn_)
                      << std::endl;
            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

}  // namespace db