# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.15)
#TODO: customize the compiler by platform type and architecture
# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
set(RENDER_MODULE ON)
# Set the binary directory path
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/out/build")

# Set the install directory path (used by 'make install' if needed)
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out/install")

project ("Noyx")

# Include sub-projects.
add_subdirectory ("Noyx")