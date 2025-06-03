# CMakeList.txt : logic CMake project for Noyx, include source and define compilators, OS, creates constants for the engine
# project specific defines here

cmake_minimum_required(VERSION 3.15)
project(NoyxEngine LANGUAGES CXX)
 
set(CMAKE_CXX_STANDARD 20)#should base on other things, TODO: change CMAKE_CXX_STANDARD
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#will collect all libraries, based on options
set(DynamicLibraries)
#     ^
#     |                                                                immer ON
#idea: RendererModule(ON/OFF), PhysicModule(ON/OFF), AIModule(ON/OFF), CoreModule(ON), AudioModule(ON/OFF), InputModule(ON/OFF)

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
        message(STATUS "  Architecture: x86")
        add_compile_definitions(NOYX_PLATFORM_WINDOWS NOYX_ARCH_X86)
    endif()
    
    add_executable (Noyx WIN32 "Noyx.cpp")                              #TODO: change "Noyx.cpp"
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
        set(LINUX TRUE CACHE INTERNAL "Is LINUX(64bit|32bit)")
    else()
        message(STATUS "  Architecture: x86")
        add_compile_definitions(NOYX_PLATFORM_LINUX NOYX_ARCH_X86)
        set(LINUX TRUE CACHE INTERNAL "Is LINUX(64bit|32bit)")
    endif()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Noyx)
    # Можна знайти Vulkan SDK через find_package(Vulkan REQUIRED)
    find_package(Vulkan REQUIRED)
    target_include_directories(NoyxCore PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(NoyxCore PRIVATE ${Vulkan_LIBRARIES})

elseif (APPLE) # in plans
    message(STATUS "Detected platform: Apple")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "  Architecture: x64 (Intel) or ARM64 (Apple Silicon)")
        add_compile_definitions(NOYX_PLATFORM_APPLE NOYX_ARCH_X64)
        set(APPLE_PLATFORM TRUE CACHE INTERNAL "Is APPLE")
    else()
        message(STATUS "  Architecture: x86 (32‑bit)")
        add_compile_definitions(NOYX_PLATFORM_APPLE NOYX_ARCH_X86)
        set(APPLE_PLATFORM TRUE CACHE INTERNAL "Is APPLE")
    endif()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Noyx)
    # macOS: підключити фреймворки, наприклад, MoltenVK чи інші
    find_library(COCOA_FRAMEWORK Cocoa)
    target_link_libraries(NoyxCore PRIVATE ${COCOA_FRAMEWORK})

else()
    message(FATAL_ERROR "Undefined platform!")
endif()

if(${DynamicLibraries})
    #all setings will be used in this subdirectory:
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source) 
    add_library(NoyxLibrary INTERFACE)
    target_link_libraries(NoyxLibrary INTERFACE
        ${DynamicLibrary}
    )
else()
    message(FATAL_ERROR "DynamicLibraries were empty, see logs of building for more infomation")
endif()


# TODO: Add tests and install targets if needed.
