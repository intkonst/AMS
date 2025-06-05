#pragma once

#include <vector>
#include <nlohmann/json_fwd.hpp>
#include <string>

#include "record.h"

namespace db {
    struct RecordVector : std::vector<Record> {
        RecordVector(const nlohmann::json& source);
        std::vector<std::string> insertQueries() const;
    };
}  // namespace db
