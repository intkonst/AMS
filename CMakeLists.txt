cmake_minimum_required(VERSION 3.23)

project(
    AMS
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AMS_SOURCES
    src/main.cc
    src/api/api.cc
    src/handler/handler.cc
    src/handler/net/net.cc
    src/handler/db/record.cc
    src/handler/db/record_vector.cc
)

set(AMS_HEADERS
    src/api/api.h
    src/handler/handler.h
    src/handler/net/net.h
    src/handler/db/record.h
    src/handler/db/record_vector.h
)

include(FetchContent)

FetchContent_Declare(
    json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE 
)

FetchContent_MakeAvailable(json)

add_executable(${PROJECT_NAME}
    ${AMS_SOURCES}
    ${AMS_HEADERS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.bin
)
