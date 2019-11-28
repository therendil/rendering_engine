cmake_minimum_required( VERSION 3.15.0 ) # Latest version of CMake when this file was created.

#set( HEADER_FILES
#    include/Window.h
#	include/Application.h
#	include/Graphics/GraphicsAPI.h
#	include/Graphics/RenderFactory.h
#	include/Graphics/RenderingCore.h
#	include/Graphics/DX12RenderFactory.h
#	include/Graphics/DX12RenderingCore.h
#)
#
#set( SRC_FILES
#    src/main.cpp
#	src/Window.cpp
#	src/Application.cpp
#	src/Graphics/GraphicsAPI.cpp
#	src/Graphics/RenderFactory.cpp
#	src/Graphics/RenderingCore.cpp
#	src/Graphics/DX12RenderFactory.cpp
#	src/Graphics/DX12RenderingCore.cpp
#)

set(HEADER_DIRS
	include
	include/Graphics
	include/Graphics/DX12
)

set(SRC_DIRS
	src
	src/Graphics
	src/Graphics/DX12
)

file(GLOB SRC_FILES
	${HEADER_DIRS}/*.h
)

file(GLOB SRC_FILES
	${SRC_DIRS}/*.cpp
)

add_executable( RenderingEngine WIN32
    ${HEADER_FILES} 
    ${SRC_FILES}
)

set_property(TARGET RenderingEngine PROPERTY CXX_STANDARD 17)

target_include_directories( RenderingEngine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries( RenderingEngine
    d3d12.lib
    dxgi.lib
    dxguid.lib
)