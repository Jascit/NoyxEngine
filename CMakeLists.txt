cmake_minimum_required(VERSION 3.20)

project(Noyx LANGUAGES CXX)

# Standard C++ Version als Variable mit Cache, um sie über GUI änderbar zu machen
set(DEFAULT_CXX_STANDARD 23 CACHE STRING "Default C++ standard for all targets")

set(CMAKE_CXX_STANDARD ${DEFAULT_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# MSVC Hot Reload
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
      "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,
      $<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,
      $<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

option(RENDER_MODULE "Enable rendering module" ON)

# Subdirectory mit Quellcode
add_subdirectory(Noyx)
