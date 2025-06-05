# CMakeLists.txt for the Core module

file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(CoreModule STATIC ${CORE_SRC})

target_include_directories(CoreModule PUBLIC
    ${CORE_INCLUDES} 
)

# Adding CoreModule to DynamicModules
list(APPEND DynamicLibraries CoreModule)

# Передача DynamicModules у батьківський контекст
set(DynamicLibraries ${DynamicLibraries} PARENT_SCOPE)
