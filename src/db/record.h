#pragma once

#include <ctime>
#include <string>

namespace db {
    class Record {
      private:
        std::string name;
        double temperature;
        double humidity;
        double brightness;
        bool test;
        std::time_t time;

      public:
        Record() = default;
        Record(
            const std::string& name, double temperature, double humidity, double brightness,
            bool test, std::time_t time
        );

        std::string insertQuery() const;
    };
}  // namespace db