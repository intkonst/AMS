cmake_minimum_required(VERSION 3.23)

project(
    AMS
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(AMS_SOURCES
    src/main.cc
    src/api/api.cc
    src/handler/handler.cc
    src/handler/db/record.cc
    src/handler/db/record_vector.cc
)

set(AMS_HEADERS
    src/api/api.h
    src/handler/handler.h
    src/handler/db/record.h
    src/handler/db/record_vector.h
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
    GIT_SHALLOW TRUE 
)

FetchContent_MakeAvailable(nlohmann_json)

add_executable(${PROJECT_NAME}
    ${AMS_SOURCES}
    ${AMS_HEADERS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.bin
)
