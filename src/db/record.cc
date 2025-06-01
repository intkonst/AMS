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
        return "INSERT INTO measuremnts (tstamp, device_id, temperature, humidity, brightness, "
               "test) "
               "VALUES (" +
               std::to_string(time) + ", " + name + ", " + std::to_string(temperature) + ", " +
               std::to_string(humidity) + ", " + std::to_string(brightness) + ", " +
               std::to_string(test) + ")";
    }
}  // namespace db
