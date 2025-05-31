#include "record.h"

Record::Record() : temperature(0), humidity(0), brightness(0), test(0), time(0) {}

Record::Record(double temperature, double humidity, double brightness, bool test, std::time_t time)
    : temperature(temperature), humidity(humidity), brightness(brightness), test(test), time(time) {
}

Record::Record(const Record& other)
    : temperature(other.temperature), humidity(other.humidity), brightness(other.brightness),
      test(other.test), time(other.time) {}

Record& Record::operator=(const Record& other) {
    temperature = other.temperature;
    humidity = other.humidity;
    brightness = other.brightness;
    test = other.test;
    time = other.time;
    return *this;
}

std::string Record::insertQuery() const {
    return "INSERT INTO database (temperature, humidity, brightness, test, time) VALUES (" +
           std::to_string(temperature) + ", " + std::to_string(humidity) + ", " +
           std::to_string(brightness) + ", " + std::to_string(test) + ", " + std::to_string(time) +
           ")";
}
