cmake_minimum_required(VERSION 3.25)
project(NoyxTests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/core")
add_executable (NoyxTests "${CMAKE_CURRENT_SOURCE_DIR}/tests_details/test_main.cpp")    