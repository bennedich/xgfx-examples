#pragma once

#include <SDL.h>

#ifdef __EMSCRIPTEN__
	//#include <SDL_opengl.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#include <emscripten.h>
#else
	#include <glad/glad.h>
#endif

typedef uint32_t u32;
