#CMakeLists for platform module

add_library(PlatformModule STATIC ${PLATFORM_SRC})

target_include_directories(PlatformModule PUBLIC
    ${PLATFORM_INCLUDES} 
)

# Adding CoreModule to DynamicModules
list(APPEND DynamicModules PlatformModule)

# Передача DynamicModules у батьківський контекст
set(DynamicModules ${DynamicModules} PARENT_SCOPE)
