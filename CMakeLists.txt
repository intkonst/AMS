cmake_minimum_required(VERSION 3.23)

project(
    AMS
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ADD_G3LOG_UNIT_TEST OFF CACHE BOOL "Skip g3log unit tests")
set(ADD_FATAL_EXAMPLE OFF CACHE BOOL "Skip fatal example")
set(ADD_G3LOG_PERFORMANCE OFF CACHE BOOL "Skip performance tests")


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

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.json
    ${CMAKE_CURRENT_SOURCE_DIR}/.bin/config.json  
    COPYONLY
)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_Declare(
    json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE 
)

FetchContent_Declare(
    spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.15.3.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_MakeAvailable(googletest json spdlog)

add_executable(${PROJECT_NAME}
    ${AMS_SOURCES}
    ${AMS_HEADERS}
)

set(LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.bin/log")

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${LOG_DIR}"
    COMMENT "Removing old log directory..."
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/config.json
        ${CMAKE_CURRENT_SOURCE_DIR}/.bin/config.json
    COMMENT "Copying input files..."
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${LOG_DIR}"
    COMMENT "Removing old log directory..."
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.bin
)
