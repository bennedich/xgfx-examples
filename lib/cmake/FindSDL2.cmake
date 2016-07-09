# Modified from:
# https://raw.githubusercontent.com/mertcanekiz/Muhittin/master/cmake/FindSDL2.cmake
# C:\Program Files\SDL2
#                      \SDL2.lib # 64 bit version from SDL2-devel-2.0.3-VC.zip
#                      \include

set(SDL2_SEARCH_PATHS
	$ENV{ProgramW6432}/SDL2
	# TODO: Fix for OSX
)

find_path(SDL2_INCLUDE_DIRS SDL.h
	PATHS ${SDL2_SEARCH_PATHS}
	PATH_SUFFIXES include/SDL2 include
	DOC "The directory where SDL.h resides"
)

find_library(SDL2_LIBRARIES
	NAMES SDL2 SDL2main
	PATHS ${SDL2_SEARCH_PATHS}
	PATH_SUFFIXES lib lib64 lib/x86 lib/x64
	DOC "The SDL2 library"
)

if(SDL2_INCLUDE_DIRS AND SDL2_LIBRARIES)
	set(SDL2_FOUND TRUE)
	message(STATUS "Looking for SDL2 - found")
else()
	set(SDL2_FOUND FALSE)
	message(STATUS "Looking for SDL2 - not found")
endif()
