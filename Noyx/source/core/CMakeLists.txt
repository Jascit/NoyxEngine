# CMakeLists.txt for the Core module

file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(CoreModule STATIC ${CORE_SRC})

target_include_directories(CoreModule PUBLIC
    ${CORE_INCLUDES}
)

# --- Detect platform & architecture and prepare per-file compile definitions ---
set(PLATFORM_COMPILE_DEFS "")   # will collect names like NOYX_PLATFORM_WINDOWS; NOYX_ARCH_X64

if (WIN32)
    if (CMAKE_HOST_SYSTEM_VERSION VERSION_GREATER_EQUAL "10.0.22000.0")
        set(IS_WIN11 TRUE CACHE INTERNAL "Is Windows 11")
    else()
        set(IS_WIN10 TRUE CACHE INTERNAL "Is Windows 10")
    endif()
    message(STATUS "Detected platform: Windows")

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "  Architecture: x64")
        list(APPEND PLATFORM_COMPILE_DEFS NOYX_PLATFORM_WINDOWS NOYX_ARCH_X64)
    else()
        message(FATAL_ERROR " Architecture x86 was found! (x86 unsupported)")
    endif()

    if(MSVC)
        set(WIN_SDK_DIR "$ENV{WindowsSdkDir}")
        set(WIN_SDK_VER "$ENV{WindowsSDKVersion}")
        if(NOT WIN_SDK_DIR OR NOT WIN_SDK_VER)
            message(FATAL_ERROR "User uses Windows, but Windows SDK wasn't found in IDE")
        else()
            set(WIN_SDK_INCLUDE "${WIN_SDK_DIR}Include/${WIN_SDK_VER}")
            set(WIN_SDK_LIB "${WIN_SDK_DIR}Lib/${WIN_SDK_VER}")
            target_include_directories(CoreModule PRIVATE "${WIN_SDK_INCLUDE}")
            target_link_libraries(CoreModule PRIVATE kernel32.lib user32.lib)
        endif()
    endif()

elseif (UNIX AND NOT APPLE)
    message(STATUS "Detected platform: Linux/Unix")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "  Architecture: x64")
        list(APPEND PLATFORM_COMPILE_DEFS NOYX_PLATFORM_LINUX NOYX_ARCH_X64)
        set(LINUX_64 TRUE CACHE INTERNAL "Is LINUX(64bit)")
    else()
        message(FATAL_ERROR " Architecture x86 was found! (x86 unsupported)")
    endif()

    # Optional: find Vulkan and attach to CoreModule
    find_package(Vulkan REQUIRED)
    if(Vulkan_FOUND)
        target_include_directories(CoreModule PRIVATE ${Vulkan_INCLUDE_DIRS})
        target_link_libraries(CoreModule PRIVATE ${Vulkan_LIBRARIES})
    endif()

elseif (APPLE)
    message(STATUS "Detected platform: Apple")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "  Architecture: 64-bit (Intel or Apple Silicon)")
        list(APPEND PLATFORM_COMPILE_DEFS NOYX_PLATFORM_APPLE NOYX_ARCH_X64)
        set(APPLE_PLATFORM_64 TRUE CACHE INTERNAL "Is APPLE(64bit)")
    else()
        message(FATAL_ERROR " Architecture x86 was found! (x86 unsupported)")
    endif()

    # Example: link Cocoa (adjust if needed)
    find_library(COCOA_FRAMEWORK Cocoa)
    if(COCOA_FRAMEWORK)
        target_link_libraries(CoreModule PRIVATE ${COCOA_FRAMEWORK})
    endif()

else()
    message(FATAL_ERROR "Undefined platform!")
endif()

# --- Apply compile defs only to platform/Platform.cpp ---
set(PLATFORM_CPP "${CMAKE_CURRENT_SOURCE_DIR}/platform/Platform.cpp")

if (EXISTS "${PLATFORM_CPP}")
    # Convert list of names into compiler options for MSVC and others.
    # For MSVC we need /DNAME, for others -DNAME
    set(MSVC_DEFS "")
    set(NONMSVC_DEFS "")
    foreach(def IN LISTS PLATFORM_COMPILE_DEFS)
        list(APPEND MSVC_DEFS "/D${def}=1")
        list(APPEND NONMSVC_DEFS "-D${def}=1")
    endforeach()

    # Join into semi-colon separated strings for COMPILE_OPTIONS property
    string(JOIN ";" MSVC_DEFS_STR ${MSVC_DEFS})
    string(JOIN ";" NONMSVC_DEFS_STR ${NONMSVC_DEFS})

    # Use generator expressions to apply correct flag syntax depending on compiler
    set(COMPILE_OPTS
        "$<$<CXX_COMPILER_ID:MSVC>:${MSVC_DEFS_STR}>"
        "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:${NONMSVC_DEFS_STR}>"
    )

    # Apply options only to that source file
    set_source_files_properties("${PLATFORM_CPP}" PROPERTIES
        COMPILE_OPTIONS "${COMPILE_OPTS}"
    )

    message(STATUS "Applied per-file compile definitions to: ${PLATFORM_CPP}")
    message(STATUS "  Definitions: ${PLATFORM_COMPILE_DEFS}")
else()
    message(WARNING "Platform.cpp not found at ${PLATFORM_CPP} Ч no per-file macros applied.")
endif()

# Adding CoreModule to DynamicModules
list(APPEND DynamicLibraries CoreModule)

# ѕередача DynamicModules у батьк≥вський контекст
set(DynamicLibraries ${DynamicLibraries} PARENT_SCOPE)