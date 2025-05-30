#include "record.h"


Record::Record(std::string name, double temperature, double humidity, double brightness, bool test, std::time_t time) :
name(name), temperature(temperature), humidity(humidity), brightness(brightness), test(test), time(time) {}

Record::Record(const Record& other) : name(other.name), temperature(other.temperature), humidity(other.humidity), brightness(other.brightness),
test(other.test), time(other.time) {}

Record& Record::operator=(const Record& other) {
    name = other.name;
    temperature = other.temperature;
    humidity = other.humidity;
    brightness = other.brightness;
    test = other.test;
    time = other.time;
    return *this;
}

std::string Record::insertQuery() const {
    return "INSERT INTO measuremnts (tstamp, device_id, temperature, humidity, brightness, test) VALUES (" + std::to_string(time) +
    ", " + name + ", " + std::to_string(temperature) + ", " + std::to_string(humidity) + ", " + std::to_string(brightness) + ", " +
    std::to_string(test) + ")";
}
