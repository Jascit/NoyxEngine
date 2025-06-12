#Top-level CMake
cmake_minimum_required(VERSION 3.25)

project(ProjectRoot)

#detecting platform and architecture
if (WIN32) # working on
  if (CMAKE_HOST_SYSTEM_VERSION VERSION_GREATER_EQUAL "10.0.22000.0")
    set(IS_WIN11 TRUE CACHE INTERNAL "Is Windows 11")
  else()
    set(IS_WIN10 TRUE CACHE INTERNAL "Is Windows 10")
  endif()
    message(STATUS "Detected platform: Windows")

    # architecture
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "  Architecture: x64")
        add_compile_definitions(NOYX_PLATFORM_WINDOWS NOYX_ARCH_X64)
    else()
        message(FATAL_ERROR " Architecture x86 was found!")
    endif()

    # compiler
    if(MSVC)
        set(WIN_SDK_DIR "$ENV{WindowsSdkDir}")
        set(WIN_SDK_VER "$ENV{WindowsSDKVersion}")
        if(NOT WIN_SDK_DIR OR NOT WIN_SDK_VER)
            message(FATAL_ERROR "User uses Windows, but Windows SDK wasn't found in IDE")
        else()
            set(WIN_SDK_INCLUDE "${WIN_SDK_DIR}Include/${WIN_SDK_VER}")
            set(WIN_SDK_LIB "${WIN_SDK_DIR}Lib/${WIN_SDK_VER}")
        endif()
    endif()
    #TODO: Clang/gcc
    
elseif (UNIX AND NOT APPLE) # in plans
    message(STATUS "Detected platform: Linux/Unix")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "  Architecture: x64")
        add_compile_definitions(NOYX_PLATFORM_LINUX NOYX_ARCH_X64)
        set(LINUX_64 TRUE CACHE INTERNAL "Is LINUX(64bit)")
    else()
        message(FATAL_ERROR " Architecture x86 was found!")
    endif()
    # Можна знайти Vulkan SDK через find_package(Vulkan REQUIRED)
    find_package(Vulkan REQUIRED)
    target_include_directories(NoyxCore PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(NoyxCore PRIVATE ${Vulkan_LIBRARIES})

elseif (APPLE) # in plans
    message(STATUS "Detected platform: Apple")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "  Architecture: x64 (Intel) or ARM64 (Apple Silicon)")
        add_compile_definitions(NOYX_PLATFORM_APPLE NOYX_ARCH_X64)
        set(APPLE_PLATFORM_64 TRUE CACHE INTERNAL "Is APPLE(64bit)")
    else()
        message(FATAL_ERROR " Architecture x86 was found!")
    endif()
    # macOS: підключити фреймворки, наприклад, MoltenVK чи інші
    find_library(COCOA_FRAMEWORK Cocoa)
    target_link_libraries(NoyxCore PRIVATE ${COCOA_FRAMEWORK})

else()
    message(FATAL_ERROR "Undefined platform!")
endif()

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