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

    /*RecordVector::RecordVector(const nlohmann::json& source) {
        const auto& devices = source["devices"];

        reserve(devices.size());

        std::tm t = {};
        std::string str_time = source["timestamp"];
        std::istringstream ss(str_time);
        ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
        std::time_t time = std::mktime(&t);

        for (const auto& device : devices.items()) {
            const auto& device_value = device.value();

            emplace_back(
                device.key(), device_value["temperature"], device_value["humidity"],
                device_value["brightness"], device_value["test"], time
            );
        }
    }*/

    std::vector<std::string> RecordVector::insertQueries() const {
        std::vector<std::string> result;

        result.reserve(size());

        for (const auto& value : *this) {
            result.push_back(value.insertQuery());
        }

        return result;
    }

}  // namespace db
