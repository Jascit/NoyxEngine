message(STATUS "\n-- ------COREMODULE TEST CMAKE------")
set(CORE_INCLUDES "${CMAKE_SOURCE_DIR}/include/")
set(CORE_ALL_LIBRARIES_DYNAMIC "")

add_subdirectory(containers)
add_subdirectory(utils)
add_subdirectory(platform)
add_subdirectory(memory)
message(STATUS "COREMODULE_TEST: creating library CORE_ALL_LIBRARIES...")
add_library(CORE_ALL_LIBRARIES INTERFACE)

target_link_libraries(CORE_ALL_LIBRARIES INTERFACE
    ${CORE_ALL_LIBRARIES_DYNAMIC}
    ${CORE_MODULE_TEST}
)
message(STATUS "COREMODULE_TEST: [SUCCESS] return library in PARENT_SCOPE(TESTS TOP CMAKE)\n")
set(TESTS_ALL_LIBRARIES_DYNAMIC ${TESTS_ALL_LIBRARIES_DYNAMIC} CORE_ALL_LIBRARIES PARENT_SCOPE)
