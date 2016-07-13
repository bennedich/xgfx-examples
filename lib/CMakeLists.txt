cmake_minimum_required( VERSION 2.8.4 )
project( lib )

if( EMSCRIPTEN )
	set( DEPENDENCIES_INCLUDES
		${CMAKE_CURRENT_SOURCE_DIR}/glm-0.9.7.5
		${CMAKE_CURRENT_SOURCE_DIR}/emscripten-renderer/include
		${CMAKE_CURRENT_SOURCE_DIR}/xecs
		PARENT_SCOPE
	)

	set( DEPENDENCIES_LIBRARIES
		${CMAKE_CURRENT_SOURCE_DIR}/emscripten-renderer/bin/web/libraphael.a
		PARENT_SCOPE
	)
else()
	set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH} )

	find_package( SDL2 REQUIRED )

	set( DEPENDENCIES_INCLUDES
		${SDL2_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/glad/include
		${CMAKE_CURRENT_SOURCE_DIR}/glm-0.9.7.5
		${CMAKE_CURRENT_SOURCE_DIR}/emscripten-renderer/include
		${CMAKE_CURRENT_SOURCE_DIR}/xecs
		PARENT_SCOPE
	)

	set( DEPENDENCIES_LIBRARIES
		${SDL2_LIBRARIES}
		${CMAKE_CURRENT_SOURCE_DIR}/emscripten-renderer/bin/desktop/libraphael.a
		PARENT_SCOPE
	)
endif()
