#include <memory>
#include <iostream>
#include <sstream>

#include "headers_type.h" // TODO Move this into a namespace.
#include "headers_gl.h"   // TODO Move this into a namespace.

#include <xecs_application.h>
#include <xecs_entity_manager.h>

#include "time.h"
#include "input_context.h"
#include "render_funcs.h"

#include "component_transform.h"
#include "component_physics.h"
#include "component_camera.h"

#include "system_tick.h"
#include "system_input.h"
#include "system_movement.h"
#include "system_camera.h"
#include "system_entrypoint.h"
#include "system_render.h"

using std::cout;
using std::endl;

namespace ReturnStatus
{
	enum _
	{
		OK,
		FAIL_INIT_SDL,
		FAIL_CREATE_SDL_WINDOW,
		FAIL_CREATE_SDL_GL_CONTEXT,
	};
}

namespace
{
	SDL_Window*   sdl_window;
	SDL_GLContext sdl_gl_context;

	std::pair< SDL_GLattr, int > GL_ATTRIBS[] = {
		{ SDL_GL_RED_SIZE,     8 },
		{ SDL_GL_GREEN_SIZE,   8 },
		{ SDL_GL_BLUE_SIZE,    8 },
		{ SDL_GL_ALPHA_SIZE,   8 },
		{ SDL_GL_DEPTH_SIZE,   8 },
		{ SDL_GL_DOUBLEBUFFER, 1 },
#if defined(__APPLE__) || defined(_WIN32)
//		{ SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_CORE }
		{ SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_COMPATIBILITY },
		{ SDL_GL_CONTEXT_MAJOR_VERSION, 2 },
		{ SDL_GL_CONTEXT_MINOR_VERSION, 1 },
#endif
	};

	std::string opengl_description()
	{
		std::ostringstream description;
		description << "OpenGL env" << std::endl;
		description << "   Version: " << glGetString( GL_VERSION ) << std::endl;
		description << "    Vendor: " << glGetString( GL_VENDOR ) << std::endl;
		description << "  Renderer: " << glGetString( GL_RENDERER ) << std::endl;
		description << "   Shading: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
		return description.str();
	}
}

// int main( int argc, char* args[] )
int main()
{
	if ( SDL_Init(SDL_INIT_VIDEO) )
	{
		cout << "Unable to initialize SDL: " << SDL_GetError() << endl;
		return ReturnStatus::FAIL_INIT_SDL;
	}

	{
		SDL_version v;
		SDL_VERSION( &v );
		printf( "SDL VERSION: %u.%u.%u\n", v.major, v.minor, v.patch );
	}

	for ( auto attr : GL_ATTRIBS )
	{
		if ( SDL_GL_SetAttribute( attr.first, attr.second ) != 0 )
		{
			cout << "FAIL SDL_GL_SetAttribute("
			<< attr.first << ", " << attr.second
			<< ") SDL Error: '" << SDL_GetError() << "'" << endl;
		}
	}

	sdl_window = SDL_CreateWindow(
		"SDL Tutorial",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		960, 480,
		SDL_WINDOW_OPENGL
//		SDL_WINDOW_ALLOW_HIGHDPI |
//		SDL_WINDOW_BORDERLESS |
//		SDL_WINDOW_RESIZABLE
	);
	if ( !sdl_window )
	{
		cout << "SDL window could not be created: '" << SDL_GetError() << "'" << endl;
		return ReturnStatus::FAIL_CREATE_SDL_WINDOW;
	}

	sdl_gl_context = SDL_GL_CreateContext( sdl_window );
	if ( !sdl_gl_context )
	{
		cout << "SDL GL context could not be created: '" << SDL_GetError() << "'" << endl;
		return ReturnStatus::FAIL_CREATE_SDL_GL_CONTEXT;
	}

	SDL_GL_MakeCurrent( sdl_window, sdl_gl_context );

#ifndef __EMSCRIPTEN__
	gladLoadGLLoader( SDL_GL_GetProcAddress );
#endif

	cout << opengl_description() << endl;

	auto stay_alive = std::make_shared<bool>();
	auto time = std::make_shared<Time>();
	auto input_context = std::make_shared<InputContext>();
	auto render_queue = std::make_shared<RenderQueue>();
	auto render_funcs = std::make_shared<RenderFuncs>( render_funcs_OpenGL );

	auto e_mgr = std::make_shared< xecs::EntityManager >();
	auto c_tfm = std::make_shared< ComponentTransform >( 32 );
	auto c_phy = std::make_shared< ComponentPhysics >( 32 );
	auto c_cam = std::make_shared< ComponentCamera >( 32 );

	std::vector< std::shared_ptr<xecs::System> > systems;

	systems.push_back( std::make_shared<SystemTick>(time) );
	systems.push_back( std::make_shared<SystemInput>(stay_alive, input_context) );
	systems.push_back( std::make_shared<SystemMovement>(time, input_context, c_tfm, c_phy, c_cam) );
	systems.push_back( std::make_shared<SystemCamera>(sdl_window, c_tfm, c_cam) );
	systems.push_back( std::make_shared<SystemEntrypoint>(render_funcs, render_queue, e_mgr, c_tfm, c_phy, c_cam) );
	systems.push_back( std::make_shared<SystemRender>(sdl_window, render_queue, c_cam) );

	xecs::Application app( stay_alive, systems );
	app.run();

	SDL_Quit();

	cout << "Shutdown" << endl;
	return ReturnStatus::OK;
}
