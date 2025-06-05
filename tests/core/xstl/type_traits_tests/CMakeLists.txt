file(GLOB_RECURSE XSTL_TT_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(XSTL_TT_TEST_SOURCES)
    add_executable(xstl_tt_tests ${XSTL_TT_TEST_SOURCES})

    target_include_directories(xstl_tt_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/Noyx/include
        ${CMAKE_CURRENT_SOURCE_DIR}/test_includes
    )

    # Falls du eigene Libraries hast (z. B. noyx_core, xstl):
    # target_link_libraries(xstl_tt_tests PRIVATE xstl_library)

    enable_testing()

endif()