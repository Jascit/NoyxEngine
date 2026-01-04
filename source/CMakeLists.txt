cmake_minimum_required(VERSION 3.25)
project(CoreModuleProject LANGUAGES CXX)

file(GLOB_RECURSE ALL_CPP CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

set(CORE_SRC "")
foreach(f ${ALL_CPP})
    file(RELATIVE_PATH rel "${CMAKE_CURRENT_SOURCE_DIR}" "${f}")
    string(FIND "${rel}" "/" pos)
    if(NOT pos EQUAL -1)
        list(APPEND CORE_SRC "${f}")
    endif()
endforeach()

message(STATUS "Found cpp in subdirs: ${CORE_SRC}")

add_library(CoreModule STATIC ${CORE_SRC})

target_include_directories(CoreModule PUBLIC
    ${CORE_INCLUDES}
)

target_compile_definitions(CoreModule PUBLIC
    $<$<CONFIG:Release>:NDEBUG>
)

set_target_properties(CoreModule PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(CoreModule PRIVATE COMMON_FLAGS)
if(ENABLE_SANITIZERS)
    add_library(CoreModule_SANITIZED_OBJECTS OBJECT ${CORE_SRC})

    target_include_directories(CoreModule_SANITIZED_OBJECTS PRIVATE ${CORE_INCLUDES})
    target_compile_definitions(CoreModule_SANITIZED_OBJECTS PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
    )

    target_link_libraries(CoreModule_SANITIZED_OBJECTS PRIVATE COMMON_FLAGS)
    if(TARGET SANITIZERS)
      target_link_libraries(CoreModule_SANITIZED_OBJECTS PRIVATE SANITIZERS)
    endif()
    
    add_library(CoreModule_SANITIZED STATIC $<TARGET_OBJECTS:CoreModule_SANITIZED_OBJECTS>)
    target_include_directories(CoreModule_SANITIZED PUBLIC ${CORE_INCLUDES})
    target_link_libraries(CoreModule_SANITIZED PRIVATE COMMON_FLAGS)

    if(TARGET SANITIZERS)
      target_link_libraries(CoreModule_SANITIZED PRIVATE SANITIZERS)
    endif()

    set(CORE_MODULE_TEST CoreModule_SANITIZED PARENT_SCOPE)
else()
    set(CORE_MODULE_TEST CoreModule PARENT_SCOPE)
endif()


list(APPEND DynamicLibraries CoreModule)
set(DynamicLibraries ${DynamicLibraries} PARENT_SCOPE)
