message(STATUS "\n-- ------XSTL-TT TEST CMAKE------")
message(STATUS "XSTL-TT: founding with GLOB_RECURSE *.cpp files in current directory")
file(GLOB_RECURSE XSTL_TT_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message(STATUS "XSTL-TT: creating library XSTL_TT_TEST_LIBRARY...")
if(XSTL_TT_TEST_SOURCES)
    add_library(XSTL_TT_TEST_LIBRARY OBJECT ${XSTL_TT_TEST_SOURCES})
    target_include_directories(XSTL_TT_TEST_LIBRARY PUBLIC
        ${CMAKE_SOURCE_DIR}/Noyx/include/core/xstl/type_traits
        ${CMAKE_CURRENT_SOURCE_DIR}/test_includes
        ${TEST_DETAILS_PATH}
    )
    set(XSTL_ALL_LIBRARIES_DYNAMIC ${XSTL_ALL_LIBRARIES_DYNAMIC} $<TARGET_OBJECTS:XSTL_TT_TEST_LIBRARY> PARENT_SCOPE)
	message(STATUS "XSTL-TT: [SUCCESS] return library in PARENT_SCOPE(COREMODULE_TEST-XSTL)\n")
else()
	message(WARNING "!!!XSTL-TT: [FAILED] XSTL_ALL_LIBRARIES_DYNAMIC was empty, there's no work to do\n")
endif()