cmake_minimum_required(VERSION 3.13)
project(dm_tools)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(dm_tools
        src/phrase_to_underscore.cpp)