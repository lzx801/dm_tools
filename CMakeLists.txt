cmake_minimum_required(VERSION 3.13)
project(dm_tools)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(dm_tools
        src/generate_NER_json.cpp
        src/underscore_text.cpp
        src/clean_non_ascii.cpp)