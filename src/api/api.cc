#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "api.h"

#define json_type "application/json"
#define html_type "text/html"


namespace {
    const std::string CONFIG_FILE_NAME = "config.json";
    std::ostringstream out;
}  // namespace

namespace api {
    void threadLoadExample() { std::this_thread::sleep_for(std::chrono::milliseconds(3000)); }

    Server::Server(db::Database& database, std::string host, int port) : database(database), host(host), port(port), server() {}

    void Server::run() {
        auto& database = this->database;
        server.Get("/api", [&database](const httplib::Request& req, httplib::Response& res) {
            std::string query = "SELECT * FROM measurements";
            std::vector<std::string> keys = {"id", "tstamp", "device_id", "temperature", "humidity", "brightness", "test"};

            if (!req.params.empty()) {
                auto isCorrect = [keys](std::string value) {
                    for (auto &&key : keys) {
                        if (key == value) {
                            return true;
                        }
                    }
                    return false;
                };

                bool is_first = true;
                for (auto &&param : req.params) {
                    if (!isCorrect(param.first)) {
                        return 400;
                    }
                    if (is_first) {
                        query += " WHERE ";
                        is_first = false;
                    } else {
                        query += " AND ";
                    }
                    query += param.first + param.second;
                }
            }
            query += ";";

            PGresult* data = database.executeQuery(query);
            nlohmann::json json;
            for (int i = 0; i < PQntuples(data); ++i) {
                for (int j = 0; j < PQnfields(data); ++j) {
                    json[i][keys[j]] = std::string(PQgetvalue(data, i, j));
                }
            }

            res.set_content(json.dump(), json_type);

            PQclear(data);
        });

        server.Get("/", [&database](const httplib::Request& req, httplib::Response& res) {
            std::string query = "SELECT * FROM measurements;";
            // std::vector<std::string> keys = {"id", "tstamp", "device_id", "temperature", "humidity", "brightness", "test"};
            std::string html = R"(
                <!DOCTYPE html>
                <html>
                <head>
                    <style>
                        table { border-collapse: collapse; width: 100%; }
                        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                        th { background-color: #f2f2f2; }
                        tr:nth-child(even) { background-color: #f9f9f9; }
                    </style>
                </head>
                <body>
                    <form action="/request"><button type="submit">Add filter</button>
                    <table>
            )";

            PGresult* data = database.executeQuery(query);

            for (int i = 0; i < PQntuples(data); ++i) {
                html += "<tr>";
                for (int j = 0; j < PQnfields(data); ++j) {
                    html += "<td>" + std::string(PQgetvalue(data, i, j)) + "</td>";
                }
                html += "</tr>";
            }

            html += R"(
                </table>
            </body>
            </html>
            )";

            res.set_content(html, html_type);
        });

        server.Get("/request", [](const httplib::Request& req, httplib::Response& res) {
            std::string html = R"(
            <form method="post" action="/">
                <div class="form-group">
                    <label for="name">Device name condition:</label>
                    <input type="text" id="device_id" name="device_id">
                </div>
                <div class="form-group">
                    <label for="text">Temperatue condition:</label>
                    <input type="text" id="temperature" name="temperature">
                </div>
                                <div class="form-group">
                    <label for="text">Humbidity condition:</label>
                    <input type="text" id="humidity" name="humidity">
                </div>
                <div class="form-group">
                    <label for="text">Brightness condition:</label>
                    <input type="text" id="brightness" name="brightness">
                </div>
                <div class="form-group">
                    <label for="text">Test field:</label>
                    <input type="checkbox" id="test" name="test">
                </div>
                <button type="submit">Submit</button>
            </form>
            )";
            res.set_content(html, html_type);
        });

        server.Post("/", [&database](const httplib::Request& req, httplib::Response& res) {
            std::string query = "SELECT * FROM measurements";
            std::vector<std::string> keys = {"id", "tstamp", "device_id", "temperature", "humidity", "brightness", "test"};
            std::vector<std::string> fields = {"device_id", "temperature", "humidity", "brightness"};

            bool is_first = true;
            for (auto &&field : fields) {
                std::string value = req.get_param_value(field);
                std::cout << value << std::endl;

                if (!value.empty()) {
                    if (is_first) {
                        query += " WHERE ";
                        is_first = false;
                    } else {
                        query += " AND ";
                    }
                    query += field + value;
                }
            }

            if (is_first) {
                query += " WHERE ";
                is_first = false;
            } else {
                query += " AND ";
            }

            query += "test=";
            query += req.get_param_value("test").empty() ? "false" : "true";
            query += ";";

            std::cout << query << std::endl;
            PGresult* data = database.executeQuery(query);

            std::string html = R"(
                <!DOCTYPE html>
                <html>
                <head>
                    <style>
                        table { border-collapse: collapse; width: 100%; }
                        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                        th { background-color: #f2f2f2; }
                        tr:nth-child(even) { background-color: #f9f9f9; }
                    </style>
                </head>
                <body>
                    <form action="/request"><button type="submit">Add filter</button>
                    <table>
            )";

            for (int i = 0; i < PQntuples(data); ++i) {
                html += "<tr>";
                for (int j = 0; j < PQnfields(data); ++j) {
                    html += "<td>" + std::string(PQgetvalue(data, i, j)) + "</td>";
                }
                html += "</tr>";
            }

            html += R"(
                </table>
            </body>
            </html>
            )";


            res.set_content(html, html_type);

            PQclear(data);
        });

        server.listen(host, port);

    }

    void apiMain() {
        /*ЧЕРНОВОЙ ВАРИАНТ ДЛЯ ТЕСТОВ, ПЕРЕДЕЛАТЬ ПОД ПОЛНОЦЕННЫЫЙ КЛАСС*/

        std::ifstream file(CONFIG_FILE_NAME);

        /*if (!file.is_open()) {*/
        if (!file) {
            std::cerr << "Error: Could not open config file at " << CONFIG_FILE_NAME << std::endl;
            return;
        }

        if (file.peek() == std::ifstream::traits_type::eof()) {
            std::cerr << "Error: Config file is empty" << std::endl;
            return;
        }

        auto config = nlohmann::json::parse(file);

        auto& logger_config = config["api"]["logger"];

        const std::string& LOGGER_NAME = logger_config["LOGGER_NAME"];
        const std::string& PATH_TO_LOGGER_FILE = logger_config["PATH_TO_LOGGER_FILE"];
        const std::string& LOGGING_LEVEL = logger_config["LOGGING_LEVEL"];
        int MAX_FILE_SIZE = logger_config["MAX_FILE_SIZE"];
        int MAX_FILES = logger_config["MAX_FILES"];

        auto api_logger =
            spdlog::rotating_logger_mt(LOGGER_NAME, PATH_TO_LOGGER_FILE, MAX_FILE_SIZE, MAX_FILES);

        api_logger->info("run api thread logger");

        out << "run api thread with id=" << std::this_thread::get_id();
        api_logger->info(out.str());
        api_logger->info("run api thread with id=%T");
        out.clear();
        api_logger->info("run load test - wait 3 sec");
        threadLoadExample();
        api_logger->info("load test completed");
    }
}  // namespace api
