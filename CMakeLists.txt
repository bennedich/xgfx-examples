cmake_minimum_required( VERSION 2.8.4 )

set( CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )

project( xgfx-examples )

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
include_directories(
	${DEPENDENCIES_INCLUDES}
	${CMAKE_SOURCE_DIR}/ex_common
)

function( ADD_TARGET target )
	add_executable( ${target} ${target}/main.cpp )
	set_property( TARGET ${target} PROPERTY CXX_STANDARD 11 )
	if( EMSCRIPTEN )
		# RELEASE
		set_target_properties( ${target} PROPERTIES LINK_FLAGS "-O2 -s USE_SDL=2" )
		# DEV
		#set_target_properties( ${target} PROPERTIES LINK_FLAGS "-s USE_SDL=2" )
	endif()
	target_link_libraries( ${target} ${DEPENDENCIES_LIBRARIES} )
endfunction( ADD_TARGET )

ADD_TARGET( ex1 )
ADD_TARGET( ex2 )
ADD_TARGET( ex3 )
