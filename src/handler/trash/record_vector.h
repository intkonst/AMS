#pragma once

#include <vector>
#include <nlohmann/json.hpp>
#include <string>

#include "record.h"


struct RecordVector : std::vector<Record> {
    RecordVector(nlohmann::json& source);
    RecordVector(const RecordVector& other);

    RecordVector& operator=(const RecordVector& other);

    std::vector<std::string> insertQuery() const;

    ~RecordVector() = default;
};
