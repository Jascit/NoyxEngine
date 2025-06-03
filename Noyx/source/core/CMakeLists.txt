# CMakeLists.txt for the Core module
cmake_minimum_required(VERSION 3.30)
project(NoyxCoreModule LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(CoreModule STATIC ${CORE_SRC})

target_include_directories(CoreModule PUBLIC
    ${CORE_INCLUDES} 
)

# Adding CoreModule to DynamicModules
list(APPEND DynamicModules CoreModule)

# Передача DynamicModules у батьківський контекст
set(DynamicModules ${DynamicModules} PARENT_SCOPE)
