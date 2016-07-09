cmake_minimum_required( VERSION 2.8.4 )

set( CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )

project( raphael-examples )

if( EMSCRIPTEN )
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/web/ )
	set( CMAKE_EXECUTABLE_SUFFIX ".html" )
	#add_definitions( -s USE_SDL=2 -s LEGACY_GL_EMULATION=1 -std=c++1y )
	add_definitions( -s USE_SDL=2 -std=c++1y )
else()
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/desktop/ )
	if( NOT WIN32 )
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -Wextra" )
	endif()
endif()

add_subdirectory( lib )
include_directories( ${DEPENDENCIES_INCLUDES} )

# ex1

add_executable( ex1 ex1/main.cpp )
set_property( TARGET ex1 PROPERTY CXX_STANDARD 11 )
if( EMSCRIPTEN )
	set_target_properties( ex1 PROPERTIES LINK_FLAGS "-s USE_SDL=2" )
endif()
target_link_libraries( ex1 ${DEPENDENCIES_LIBRARIES} )
