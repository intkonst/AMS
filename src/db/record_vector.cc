#include "record_vector.h"

const int NUMBER_OF_HANDLERS = 2;

RecordVector::RecordVector(nlohmann::json& source) {
    resize(NUMBER_OF_HANDLERS);

    std::tm t = {};
    std::string str_time = source["timestamp"];
    std::istringstream ss(str_time);
    ss >> std::get_time(&t, "%Y-%b-%dT%H:%M:%S");
    std::time_t time = std::mktime(&t);

    int number_zeroes = 4;
    for (int i = 1; i <= NUMBER_OF_HANDLERS; ++i) {
        if (i % int(std::pow(10, 5 - number_zeroes)) == 0) {
            --number_zeroes;
        }
        std::string handler = "id-";
        for (int j = 0; j < number_zeroes; ++j) {
            handler += "0";
        }
        handler += std::to_string(i);

        this->operator[](i - 1) = Record(
            source["devices"][handler]["temperature"], source["devices"][handler]["humidity"],
            source["devices"][handler]["brightness"], source["devices"][handler]["test"], time
        );
    }
}

RecordVector::RecordVector(const RecordVector& other) {
    for (auto&& i : other) {
        push_back(i);
    }
}

RecordVector& RecordVector::operator=(const RecordVector& other) {
    clear();
    for (auto&& i : other) {
        push_back(i);
    }
    return *this;
}

std::vector<std::string> RecordVector::insertQuery() const {
    std::vector<std::string> result(NUMBER_OF_HANDLERS);
    int position = 0;
    for (auto&& i : *this) {
        result[position++] = i.insertQuery();
    }

    return result;
}
