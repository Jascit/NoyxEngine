add_library(RenderModule INTERFACE)

if(IS_WIN10 OR IS_WIN11)
	set(RENDER_INCLUDES "${CMAKE_SOURCE_DIR}/Noyx/Include/render/render_DX12")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/render_DX12")
else()
	set(RENDER_INCLUDES "${CMAKE_SOURCE_DIR}/Noyx/Include/render/render_vulkan")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/render_vulkan")
endif()
