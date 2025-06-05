cmake_minimum_required(VERSION 3.16)

# Подключаем систему сборки ESP-IDF
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set_source_files_properties(main.cpp PROPERTIES LANGUAGE CXX)

project(
    AMS
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)