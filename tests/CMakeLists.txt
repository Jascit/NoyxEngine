cmake_minimum_required(VERSION 3.25)
project(NoyxTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(TEST_DETAILS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/test_framework/include")
set(TESTS_ALL_LIBRARIES_DYNAMIC "")
set(TEST_SUBDIRECTORIES "")

set(CORE_TEST_FLAG ON)
set(AVAILABLE_TEST_MODULES CORE OTHER_MODULE)
foreach(MOD IN LISTS AVAILABLE_TEST_MODULES)
    set(FLAG_NAME "${MOD}_TEST_FLAG")
    if(DEFINED ${FLAG_NAME} AND ${FLAG_NAME})
        list(APPEND TEST_SUBDIRECTORIES ${MOD})
    endif()
endforeach()

message(STATUS "------TOP_TEST CMAKE------")
foreach(SUBDIR ${TEST_SUBDIRECTORIES})
    set(SUBDIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}")
    if(EXISTS "${SUBDIR_PATH}" AND IS_DIRECTORY "${SUBDIR_PATH}")
        message(STATUS "TOP_TEST: Adding directory to test: ${SUBDIR}")
        add_subdirectory("${SUBDIR_PATH}")
    else()
        message(WARNING "Directory ${SUBDIR_PATH} does not exist.")
    endif()
endforeach()

# --- main test executable
add_executable(tests
  "${CMAKE_CURRENT_SOURCE_DIR}/test_framework/test_main.cpp"
)

target_link_libraries(tests PRIVATE COMMON_FLAGS)
if(TARGET SANITIZERS)
  target_link_libraries(tests PRIVATE SANITIZERS)
endif()

if(TESTS_ALL_LIBRARIES_DYNAMIC)
  target_link_libraries(tests PRIVATE ${TESTS_ALL_LIBRARIES_DYNAMIC})
endif()

target_include_directories(tests PUBLIC ${TEST_DETAILS_PATH})
message(STATUS "TEST: Test's binary dir: ${CMAKE_BINARY_DIR}/bin/tests")
set_target_properties(tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/tests
    RUNTIME_OUTPUT_DIRECTORY_REALEASE ${CMAKE_BINARY_DIR}/bin/tests
)

add_test(NAME tests COMMAND tests)