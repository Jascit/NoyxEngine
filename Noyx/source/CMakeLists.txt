#CMakeList.txt : CMake for libraries building, defines modules to include(based on options from top-level and logic CMakeLists)

set(SUBDIRECTORIES
    core
    platform
    render
)

set(CORE_INCLUDES "${CMAKE_SOURCE_DIR}/Noyx/Include/core")
set(PLATFORM_INCLUDES "${CMAKE_SOURCE_DIR}/Noyx/Include/platform")

foreach(SUBDIR ${SUBDIRECTORIES})
    set(SUBDIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}")
    if(EXISTS "${SUBDIR_PATH}" AND IS_DIRECTORY "${SUBDIR_PATH}")
        add_subdirectory("${SUBDIR_PATH}")
    else()
        message(WARNING "Directory ${SUBDIR_PATH} does not exist.")
    endif()
endforeach()

#will collect all libraries, based on options
set(DynamicLibraries ${DynamicLibraries} PARENT_SCOPE)
#     ^
#     |                                                                immer ON        Immer ON
#idea: RendererModule(ON/OFF), PhysicModule(ON/OFF), AIModule(ON/OFF), CoreModule(ON), PlatformModule(ON), AudioModule(ON/OFF), InputModule(ON/OFF)    
