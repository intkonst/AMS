#include "record_vector.h"
#include <iostream>


RecordVector::RecordVector(nlohmann::json& source) {
    std::tm t = {};
    std::string str_time = source["timestamp"];
    std::istringstream ss(str_time);
    ss >> std::get_time(&t, "%Y-%b-%dT%H:%M:%S");
    std::time_t time = std::mktime(&t);

    for (auto &&device : source["devices"].items()) {
        auto x = device.key();
        std::cout << x << std::endl;
        push_back(Record(device.key(), device.value()["temperature"], device.value()["humidity"],
        device.value()["brightness"], device.value()["test"], time));
    }
}

RecordVector::RecordVector(const RecordVector& other) : vector::vector(other.max_size()) {
    for (auto &&i : other) {
        push_back(i);
    }
}

RecordVector& RecordVector::operator=(const RecordVector& other) {
    clear();
    resize(other.max_size());
    for (auto &&i : other) {
        push_back(i);
    }
    return *this;
}

std::vector<std::string> RecordVector::insertQuery() const {
    std::vector<std::string> result(size());
    int position = 0;
    for (auto &&i : *this) {
        result[position++] = i.insertQuery();
    }

    return result;
}
