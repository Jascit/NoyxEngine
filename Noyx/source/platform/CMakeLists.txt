#CMakeLists for platform module

file(GLOB_RECURSE PLATFORM_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
if(GRAPHICS_SRC)
    add_library(PlatformModule STATIC ${PLATFORM_SRC})
    
    target_include_directories(PlatformModule PUBLIC
        ${PLATFORM_INCLUDES}  
    )
    
    list(APPEND DynamicLibraries PlatformModule)
    
    set(DynamicLibraries ${DynamicLibraries} PARENT_SCOPE)
endif()
