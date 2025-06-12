# CMakeList.txt : logic CMake project for Noyx, include source and define compilators, OS, creates constants for the engine

# project specific defines here

cmake_minimum_required(VERSION 3.25)
project(Noyx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DynamicLibraries "")

#all setings will be used in this subdirectory:
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source) 
#NoyxLibrary created in CMake(Source)
if(NOT ${DynamicLibraries} STREQUAL "")
    add_executable (Noyx WIN32 ${CMAKE_CURRENT_SOURCE_DIR}/source/Noyx.cpp)                              #TODO: change "Noyx.cpp"
    target_link_libraries(Noyx ${DynamicLibraries})
else()
    message(FATAL_ERROR "DynamicLibraries were empty, see logs of building for more infomation")
endif()


# TODO: Add tests and install targets if needed.
