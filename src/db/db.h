#pragma once

#include "record.h"
#include "record_vector.h"
#include <libpq-fe.h>
#include <string>


class Database {
  PGconn *conn = nullptr;
  std::string conninfo;

public:
  Database(const std::string &conninfo);
  ~Database();
  bool create_table_if_not_exists();
  bool connect();
  void printDatabase();
  bool addRecord(const Record& rec);
  bool checkDuplicates();
  bool removeDuplicates();
  bool ensureUniqueIndex();
  bool addRecords(const RecordVector& records);
  bool executeQuery(const std::string& query);
};

std::time_t string_to_tstamp(const std::string& datetime);