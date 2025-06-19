cmake_minimum_required(VERSION 3.23)

project(
    AMS
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})

set(AMS_SOURCES
    src/main.cc
    src/api/api.cc
    src/db/db.cc
    src/db/record.cc
    src/db/record_vector.cc
    src/handler/handler.cc
    src/handler/sock/udp_server.cc
)

set(AMS_HEADERS
    src/api/api.h
    src/db/db.h
    src/db/record.h
    src/db/record_vector.h
    src/handler/handler.h
    src/handler/sock/udp_server.h
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.json
    ${CMAKE_CURRENT_SOURCE_DIR}/.bin/config.json  
    COPYONLY
)

include(FetchContent)

# FetchContent_Declare(
#     googletest
#     URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
#     DOWNLOAD_EXTRACT_TIMESTAMP TRUE 
# )

FetchContent_Declare(
    cpp-httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.12.1
)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE 
)

FetchContent_Declare(
    spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.15.3.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_MakeAvailable(json spdlog cpp-httplib)

add_executable(${PROJECT_NAME}
    ${AMS_SOURCES}
    ${AMS_HEADERS}
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/config.json
        ${CMAKE_CURRENT_SOURCE_DIR}/.bin/config.json
    COMMENT "Copying input files..."
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    httplib
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    PostgreSQL::PostgreSQL
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.bin
)
