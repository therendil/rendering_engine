cmake_minimum_required( VERSION 3.9.1 ) # Latest version of CMake when this file was created.

set( HEADER_FILES
    include/Window.h
	include/Win32Window.h
)

set( SRC_FILES
    src/main.cpp
	src/Win32Window.cpp
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