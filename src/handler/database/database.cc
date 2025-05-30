#include "database.h"
#include <iostream>

Database::Database(const std::string &conninfo) : conninfo(conninfo) {}

Database::~Database() {
  if (conn) {
    PQfinish(conn);
  }
}

bool Database::connect() {
  std::cout << "Connecting with conninfo: " << conninfo << std::endl;
  conn = PQconnectdb(conninfo.c_str());

  if (conn == nullptr) {
    std::cerr << "[Database] PQconnectdb returned nullptr" << std::endl;
    return false;
  }
  if (PQstatus(conn) != CONNECTION_OK) {
    std::cerr << "[Database] Connection failed: " << PQerrorMessage(conn)
              << std::endl;
    PQfinish(conn);
    conn = nullptr;
    return false;
  }

  std::cout << "[Database] Connected successfully.\n";
  return true;
}

bool Database::create_table_if_not_exists() {
  const char *sql = "CREATE TABLE IF NOT EXISTS measurements ("
                    "id SERIAL PRIMARY KEY, "
                    "tstamp TIMESTAMP WITHOUT TIME ZONE, "
                    "device_id TEXT, "
                    "temperature REAL, "
                    "humidity REAL, "
                    "brightness REAL, "
                    "test BOOLEAN, "
                    "UNIQUE(tstamp, device_id)"
                    ");";

  PGresult *res = PQexec(conn, sql);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    std::cerr << "Create table failed: " << PQerrorMessage(conn) << std::endl;
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

    PGresult* res = PQexec(conn, sql);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Failed to check duplicates: " << PQerrorMessage(conn) << std::endl;
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

    PGresult* res = PQexec(conn, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Failed to remove duplicates: " << PQerrorMessage(conn) << std::endl;
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

    PGresult* res = PQexec(conn, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Failed to add unique constraint: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
}


void Database::printDatabase() {
  const char *query = "SELECT id, tstamp, device_id, temperature, humidity, "
                      "brightness, test FROM measurements;";
  PGresult *res = PQexec(conn, query);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    std::cerr << "SELECT failed: " << PQerrorMessage(conn) << std::endl;
    PQclear(res);
    return;
  }

  int nrows = PQntuples(res);
  int nfields = PQnfields(res);

  const int col_width = 20;

  for (int i = 0; i < nfields; i++) {
    std::cout << std::left << std::setw(col_width) << PQfname(res, i);
  }
  std::cout << "\n" << std::string(nfields * col_width, '-') << "\n";

  for (int i = 0; i < nrows; i++) {
    for (int j = 0; j < nfields; j++) {
      std::cout << std::left << std::setw(col_width) << PQgetvalue(res, i, j);
    }
    std::cout << "\n";
  }

  PQclear(res);
}

bool Database::addRecord(const Record &rec) {
  std::string query = rec.insertQuery();

  PGresult *res = PQexec(conn, query.c_str());

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    std::cerr << "[Database] Insert failed: " << PQerrorMessage(conn)
              << std::endl;
    PQclear(res);
    return false;
  }

  PQclear(res);
  return true;
}

bool Database::addRecords(const RecordVector& records) {
    bool allSuccess = true;

    for (const auto& rec : records) {
        if (!addRecord(rec)) {
            std::cerr << "[Database] Failed to insert record." << ")\n";
            allSuccess = false;
        }
    }

    return allSuccess;
}


std::time_t string_to_tstamp(const std::string &datetime) {
  std::tm tm = {};
  std::istringstream ss(datetime);

  ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
  if (ss.fail()) {
    throw std::runtime_error("Failed to parse datetime string: " + datetime);
  }
  return std::mktime(&tm);
}

bool Database::executeQuery(const std::string& query) {
    if (!conn) {
        std::cerr << "[Database] No connection to execute query.\n";
        return false;
    }

    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "[Database] Query execution failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}
