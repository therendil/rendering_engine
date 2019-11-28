cmake_minimum_required( VERSION 3.15.0 ) # Latest version of CMake when this file was created.

set( HEADER_FILES
    include/Window.h
	include/Application.h
)

set( SRC_FILES
    src/main.cpp
	src/Window.cpp
	src/Application.cpp
)

add_executable( RenderingEngine WIN32
    ${HEADER_FILES} 
    ${SRC_FILES}
)

target_include_directories( RenderingEngine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries( RenderingEngine
    d3d12.lib
    dxgi.lib
    dxguid.lib
)