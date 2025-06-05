#pragma once

#include <vector>
#include <nlohmann/json_fwd.hpp>
#include <string>

#include "record.h"

namespace db {
    struct RecordVector : std::vector<Record> {
        RecordVector(const std::vector<char*> source);
        std::vector<std::string> insertQueries() const;
    };
}  // namespace db
