#Top-level CMake
cmake_minimum_required(VERSION 3.25)

project(ProjectRoot)

if (MSVC)
  add_compile_options(/std:c++latest)
endif()

# MSVC Hot-Reload (CMP0141)
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
      "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/out/build")

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out/install")

option(RENDER_MODULE "Enable rendering module" ON)


add_subdirectory(Noyx)
#tests
add_subdirectory(tests)

