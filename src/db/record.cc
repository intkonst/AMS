#include <ctime>
#include <string>

#include "record.h"

namespace db {
    Record::Record(
        const std::string& name, double temperature, double humidity, double brightness, bool test,
        std::time_t time
    )
        : name(name), temperature(temperature), humidity(humidity), brightness(brightness),
          test(test), time(time) {}

    std::string Record::insertQuery() const {
        char string_time[20];
        std::strftime(string_time, 20, "%Y-%m-%d %H:%M:%S", gmtime(&time));
        return "INSERT INTO measurements (tstamp, device_id, temperature, humidity, brightness, "
               "test) "
               "VALUES ('" +
               std::string(string_time) + "', '" + name + "', " + std::to_string(temperature) + ", " +
               std::to_string(humidity) + ", " + std::to_string(brightness) + ", " +
               (test ? "true" : "false") + ")";
    }
}  // namespace db