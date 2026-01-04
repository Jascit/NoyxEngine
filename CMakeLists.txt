cmake_minimum_required(VERSION 3.25)
project(NoyxCore C CXX)
project(NoyxCore VERSION 1.0.0)

# Встановлюємо стандарт C++
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Collect canonical defs here
set(PLATFORM_COMPILE_DEFS "")
add_executable(main source/main.cpp)
# Basic cross-compile warning
if (CMAKE_CROSSCOMPILING)
    message(WARNING "CMake is in cross-compiling mode. Detection may reflect host or target depending on toolchain settings.")
endif()

# Detect OS
if (WIN32)
    message(STATUS "Detected platform: Windows")
    list(APPEND PLATFORM_COMPILE_DEFS PLATFORM_WINDOWS)

    # Optional: detect Windows 10 vs 11 if CMAKE_HOST_SYSTEM_VERSION is set
    if (DEFINED CMAKE_HOST_SYSTEM_VERSION)
       set(IS_WIN TRUE CACHE INTERNAL "Is Windows 10")
       list(APPEND PLATFORM_COMPILE_DEFS WINDOWS)
    endif()

elseif (APPLE)
    message(STATUS "Detected platform: Apple (macOS/iOS/etc)")
    list(APPEND PLATFORM_COMPILE_DEFS APPLE)

    # Try several reliable ways to detect processor arch on Apple:
    if (CMAKE_OSX_ARCHITECTURES) # set when user/toolchain declared it
        string(JOIN ";" _archs ${CMAKE_OSX_ARCHITECTURES})
        if (_archs MATCHES "arm64")
            list(APPEND PLATFORM_COMPILE_DEFS ARCH_ARM64)
        elseif (_archs MATCHES "x86_64")
            list(APPEND PLATFORM_COMPILE_DEFS ARCH_X64)
        endif()
    endif()

    # fallbacks
    if (NOT PLATFORM_COMPILE_DEFS MATCHES "ARCH_")
        if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
            list(APPEND PLATFORM_COMPILE_DEFS ARCH_ARM64)
        else()
            list(APPEND PLATFORM_COMPILE_DEFS ARCH_X64)
        endif()
    endif()

elseif (UNIX AND NOT APPLE)
    message(STATUS "Detected platform: Linux/Unix")
    list(APPEND PLATFORM_COMPILE_DEFS LINUX)
else()
    message(FATAL_ERROR "Undefined platform!")
endif()

# Ensure 64-bit (or fail)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    # If not already appended (Apple logic may have done arch) ensure X64 present
    if (NOT " ${PLATFORM_COMPILE_DEFS} " MATCHES " ARCH_X64 ")
        # if arm64 already present, keep it; otherwise append X64
        if (NOT " ${PLATFORM_COMPILE_DEFS} " MATCHES " ARCH_ARM64 ")
            list(APPEND PLATFORM_COMPILE_DEFS ARCH_X64)
        endif()
    endif()
else()
    message(FATAL_ERROR "32-bit targets are not supported (CMAKE_SIZEOF_VOID_P != 8)")
endif()

# Show what we detected
message(STATUS "Platform/arch compile definitions: ${PLATFORM_COMPILE_DEFS}")

# ----------------- COMMON_FLAGS INTERFACE target (single place for all flags) -----------------
add_library(COMMON_FLAGS INTERFACE)

# Put the platform/arch defines only into COMMON_FLAGS
if (PLATFORM_COMPILE_DEFS)
    target_compile_definitions(COMMON_FLAGS INTERFACE ${PLATFORM_COMPILE_DEFS})
endif()

# Sanitizer for tests
set(ENABLE_SANITIZERS OFF)
if (ENABLE_SANITIZERS AND MSVC)
    string(REPLACE "/RTC1" "" _cxx_dbg "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_DEBUG "${_cxx_dbg}" CACHE STRING "" FORCE)

    string(REPLACE "/RTC1" "" _c_dbg "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS_DEBUG "${_c_dbg}" CACHE STRING "" FORCE)

    message(STATUS "Sanitizers enabled: /RTC1 removed from Debug flags")
    # --- Create SANITIZERS interface only when requested
    add_library(SANITIZERS INTERFACE)
    if(MSVC)
      target_compile_options(SANITIZERS INTERFACE
        $<$<CONFIG:Debug>:/fsanitize=address>
      )
      target_link_options(SANITIZERS INTERFACE
        $<$<CONFIG:Debug>:/fsanitize=address>
      )
    else()
      target_compile_options(SANITIZERS INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address>
      )
      target_link_options(SANITIZERS INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address>
      )
    endif()
endif()

if(MSVC)
    # MSVC
    target_compile_options(COMMON_FLAGS INTERFACE
        # Debug
        $<$<CONFIG:Debug>:
            /Zi                 # Debug symbols
            /Od                 # Disable optimizations
            /W4                 # High warnings
            /EHsc               # Exception model
            /Oy-                # Keep frame pointers for backtrace
        >
        # Release
        $<$<CONFIG:Release>:
            /O2                 # Optimize speed
            /Ot                 # Favor fast code
            /Oi                 # Enable intrinsics
            /GL                 # Whole program optimization
            /DNDEBUG            # Disable asserts
            /W3                 # Moderate warning level
            /fp:fast            # Fast floating point
        >
    )
    if(NOT ENABLE_SANITIZERS)
      target_compile_options(COMMON_FLAGS INTERFACE 
      $<$<CONFIG:Debug>:
            /RTC1               # Runtime checks
        >
      )
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC / Clang
    target_compile_options(COMMON_FLAGS INTERFACE
        # Debug
        $<$<CONFIG:Debug>:
            -g                  # Debug symbols
            -O0                 # No optimization
            -fno-omit-frame-pointer  # Keep frame pointers
            -Wall -Wextra -Wpedantic # Warnings
            -fno-inline         # Better stack traces
            -fexceptions        # Enable exceptions
        >
        # Release
        $<$<CONFIG:Release>:
            -O3                 # Max speed optimization
            -march=native       # Optimize for current CPU
            -mtune=native
            -DNDEBUG            # Disable asserts
            -ffast-math         # Fast floating point (test carefully)
            -funroll-loops      # Optional: unroll loops
            -fstrict-aliasing   # Strict aliasing
            -flto               # Link Time Optimization
        >
    )
else()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()
# C++ standard
target_compile_features(COMMON_FLAGS INTERFACE cxx_std_23)

set(CORE_INCLUDES "${CMAKE_SOURCE_DIR}/include/" "${CMAKE_CURRENT_SOURCE_DIR}/api")
add_subdirectory(source)
target_link_libraries(main PRIVATE ${DynamicLibraries})
target_include_directories(main PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/api")
target_link_libraries(main PRIVATE COMMON_FLAGS)
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/"
)

add_subdirectory(tests)