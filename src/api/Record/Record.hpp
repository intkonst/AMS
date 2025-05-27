#include <nlohmann/json.hpp>
#include <ctime>
#include <string>


class Record
{
private:
    double temperature;
    double humidity;
    double brightness;
    bool test;
    std::time_t time;
public:
    Record(double temperature, double humidity, double brightness, bool test, std::time_t time);
    Record(const Record& other);

    Record& operator=(const Record& other);
    std::string insertQuery() const;

    ~Record() = default;
};
