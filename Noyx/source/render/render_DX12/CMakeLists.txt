# CMakeLists.txt for the DX12 module

file(GLOB_RECURSE DX12_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
if(DX12_SRC)
    add_library(DX12lib STATIC ${DX12_SRC})
    
    target_include_directories(DX12lib PUBLIC
        "${WIN_SDK_INCLUDE}/um"
        "${WIN_SDK_INCLUDE}/shared"
        "${WIN_SDK_INCLUDE}/ucrt"
        "${WIN_SDK_INCLUDE}/winrt"
         ${RENDER_INCLUDES}
         ${CORE_INCLUDES}
         ${PLATFORM_INCLUDES}
    )
    target_link_libraries(DX12lib PUBLIC
        "${WIN_SDK_LIB}/um/x64"
        "${WIN_SDK_LIB}/ucrt/x64"
    )
    
    # Adding CoreModule to DynamicModules
    list(APPEND DynamicLibraries DX12lib)
    
    # Передача DynamicModules у батьківський контекст
    set(DynamicLibraries ${DynamicLibraries} PARENT_SCOPE)
endif()