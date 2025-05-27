#include "Record.hpp"


Record::Record(double temperature=0, double humidity=0, double brightness=0, bool test=0, std::time_t time=std::time(nullptr)) :
temperature(temperature), humidity(humidity), brightness(brightness), test(test), time(time) {}

Record::Record(const Record& other) : temperature(other.temperature), humidity(other.humidity), brightness(other.brightness),
test(other.test), time(other.time) {}

Record& Record::operator=(const Record& other) {
    temperature = other.temperature;
    humidity = other.humidity;
    brightness = other.brightness;
    test = other.test;
    time = other.time;
}

std::string Record::insertQuery() const {
    return "INSERT INTO database (temperature, humidity, brightness, test, time) VALUES (" + std::to_string(temperature) +
    ", " + std::to_string(humidity) + ", " + std::to_string(brightness) + ", " + std::to_string(test) + ", " + std::to_string(time) + ")";
}
