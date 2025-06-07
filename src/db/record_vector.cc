#include <vector>

#include "record_vector.h"
#include <nlohmann/json.hpp>

#include "record.h"

namespace db {

    RecordVector::RecordVector(const std::vector<char*> source) : vector(source.size()) {
        std::time_t time = std::time(nullptr);

        for (auto &&i : source) {
            nlohmann::json json = nlohmann::json::parse(i);
            for (auto &&i : json.items()) {
                auto data = i.value();
                emplace_back(i.key(), data["temperature"], data["humidity"], data["brightness"], data["test"], time);
            }
        }
    }

    std::vector<std::string> RecordVector::insertQueries() const {
        std::vector<std::string> result;

        result.reserve(size());

        for (const auto& value : *this) {
            result.push_back(value.insertQuery());
        }

        return result;
    }

}  // namespace db
