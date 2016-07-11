#include <cstdint>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_access.hpp>    // glm::column, glm::row
#include <glm/gtc/matrix_transform.hpp> // glm::perspective
#include <glm/gtx/transform.hpp>        // glm::translate, glm::rotate, glm::scale
#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr

#include "headers_type.h"
#include "headers_gl.h"

#include "shader_loader.h"
#include "mesh_loader.h"

#include "render_key.h"
#include "render_command.h"
#include "render_queue.h"
#include "render_func.h"

// Gpu resource loaders assumes resource data has already been loaded into suitable data structure.
// Their role is to load/unload resources to the gpu side.
// Resource loaders may be virtual.
// Resource data structures mustnt be virtual.
// Manage: mesh, shader, texture

// How about creating a separate project for data stuctures?
// And a separate project for loading resources?
// ...and then a project for an ecs app?

using std::cout;
using std::endl;

using namespace Raphael;

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

struct VertexBitmap
{
	float pos[ 3 ];
	float uv[ 2 ];
};

namespace
{
	bool g_stay_alive = true;

	// Camera transform.

	glm::vec3 pos;
	glm::vec3 posv;
	glm::quat rot;
	glm::quat rotv;
	glm::mat4 transform;
	glm::mat4 proj;
	glm::mat4 view;
	glm::mat4 vp;

	u32 t0;

	// SDL and opengl.

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
//#else // RPI
//		{ SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_ES },
//		{ SDL_GL_CONTEXT_MAJOR_VERSION, 2 },
//		{ SDL_GL_CONTEXT_MINOR_VERSION, 0 },
#endif
	};

	RenderKey     quad_key;
	RenderCommand quad_cmd;
	RenderQueue   render_queue;
//	Renderer      renderer;

	VertexBitmap quad[ 4 ] = {
		{ -.5f, -.5f, .0f, 0.f, 0.f },
		{ -.5f,  .5f, .0f, 0.f, 1.f },
		{  .5f, -.5f, .0f, 1.f, 0.f },
		{  .5f,  .5f, .0f, 1.f, 1.f },
	};

#ifdef __EMSCRIPTEN__
	const std::string shader_source_prefix = R"(
		precision mediump float;
		)";
#else
	const std::string shader_source_prefix = R"(
		)";
#endif

	const std::string vert = R"(
		uniform mat4 mvp;

		attribute vec3 pos;
		//attribute vec4 col;
		attribute vec2 uv;

		varying vec4 f_color;
		varying vec2 f_uv;

		void main ()
		{
			gl_Position = mvp * vec4( pos, 1.0 );
			f_color = vec4(0, uv, 1);//col;
			f_uv = uv;
		}
	)";

	const std::string frag = R"(
		//uniform sampler2D texture;

		varying vec4 f_color;
		varying vec2 f_uv;

		void main ()
		{
			gl_FragColor = f_color;// * texture2D( texture, f_uv );
		}
	)";

	inline bool compare_render_key ( RenderKey a, RenderKey b )
	{
		return a.raw < b.raw;
	}

	void draw_bitmap( const RenderCommand* command )
	{
		glUseProgram( command->DrawBitmap.shader );

		glUniformMatrix4fv( glGetUniformLocation( command->DrawBitmap.shader, "mvp" ), 1, GL_FALSE, command->DrawBitmap.mvp );
//		glUniform2fv( glGetUniformLocation( command->DrawBitmap.shader, "size" ), 1, command->DrawBitmap.size );
//
		GLsizei stride = sizeof( VertexBitmap );
		int n_verts = command->DrawBitmap.vbo_n / stride;
		auto uv_offset = (const void*)sizeof( VertexBitmap::pos );

		glBindBuffer( GL_ARRAY_BUFFER, command->DrawBitmap.vbo );
		glVertexAttribPointer( glGetAttribLocation( command->DrawBitmap.shader, "pos" ), 3, GL_FLOAT, GL_FALSE, stride, 0 );
		glVertexAttribPointer( glGetAttribLocation( command->DrawBitmap.shader, "uv" ), 2, GL_FLOAT, GL_FALSE, stride, uv_offset );
		//glVertexAttribPointer( glGetAttribLocation( data.shader, "col" ), 3, GL_FLOAT, GL_FALSE, stride, 0 );

		// TODO bind relevant texture here.
		// TODO only activate and rebind if neccessary.
//		int texture_i = 0;
//		glActiveTexture( GL_TEXTURE0 + texture_i );
//		glUniform1i( glGetUniformLocation( command->DrawBitmap.shader, "texture" ), texture_i );

//		glBindTexture( GL_TEXTURE_2D, command->DrawBitmap.bitmap );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, n_verts );
	}
}

static std::string opengl_description()
{
	std::ostringstream description;
	description << "OpenGL env" << std::endl;
	description << "   Version: " << glGetString( GL_VERSION ) << std::endl;
	description << "    Vendor: " << glGetString( GL_VENDOR ) << std::endl;
	description << "  Renderer: " << glGetString( GL_RENDERER ) << std::endl;
	description << "   Shading: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
	return description.str();
}

static void handle_input( u32 dt )
{
	const float D_POS = dt * .001f; // Delta movement.
	const float D_ROT = dt * .0006f; // Delta rotation.
	const float RETARDATION = .15f;

	// Extract transform components right, up, forward.

	glm::vec3& r = reinterpret_cast< glm::vec3& >( transform[ 0 ] );
	glm::vec3& u = reinterpret_cast< glm::vec3& >( transform[ 1 ] );
	glm::vec3& f = reinterpret_cast< glm::vec3& >( transform[ 2 ] );

	// Translate.

	const uint8_t* state = SDL_GetKeyboardState( NULL );
	if ( state[SDL_SCANCODE_A] )
		posv -= D_POS * r;
	if ( state[SDL_SCANCODE_D] )
		posv += D_POS * r;
	if ( state[SDL_SCANCODE_F] )
		posv -= D_POS * u;
	if ( state[SDL_SCANCODE_R] )
		posv += D_POS * u;
	if ( state[SDL_SCANCODE_S] )
		posv += D_POS * f;
	if ( state[SDL_SCANCODE_W] )
		posv -= D_POS * f;

	pos += posv;
	const auto VEC3_ZERO = glm::vec3();
	posv = glm::mix( posv, VEC3_ZERO, RETARDATION );

	// Rotate.

	if ( state[SDL_SCANCODE_E] )
		rotv = glm::angleAxis( -D_ROT, f ) * rotv;
	if ( state[SDL_SCANCODE_Q] )
		rotv = glm::angleAxis( D_ROT, f ) * rotv;
	if ( state[SDL_SCANCODE_RIGHT] )
		rotv = glm::angleAxis( -D_ROT, u ) * rotv;
	if ( state[SDL_SCANCODE_LEFT] )
		rotv = glm::angleAxis( D_ROT, u ) * rotv;
	if ( state[SDL_SCANCODE_DOWN] )
		rotv = glm::angleAxis( -D_ROT, r ) * rotv;
	if ( state[SDL_SCANCODE_UP] )
		rotv = glm::angleAxis( D_ROT, r ) * rotv;

	rot = rotv * rot;
	const auto QUAT_ZERO = glm::quat();
	rotv = glm::lerp( rotv, QUAT_ZERO, RETARDATION );

	// Apply transformations.

	transform = glm::translate( glm::mat4(1), pos ) * glm::mat4_cast( rot );
	view = glm::inverse( transform );
	vp = proj * view;

	// Listen to input events.

	SDL_Event e;
	while ( SDL_PollEvent(&e) )
	{
		switch( e.type )
		{
			case SDL_QUIT:
				g_stay_alive = false;
				break;
			case SDL_KEYDOWN:
				switch( e.key.keysym.sym )
				{
					case SDLK_ESCAPE:
						g_stay_alive = false;
						break;
				}
				break;
		}
	}
}

static void render( u32 dt )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	std::sort( render_queue.key.data(), render_queue.key.data() + render_queue.size, compare_render_key );
	for ( u32 i = 0, n = render_queue.size; i < n; ++i )
	{
		int command_i = render_queue.key[ i ].RenderCommon.data_index;
		RenderCommand& cmd = render_queue.cmd[ command_i ];

		reinterpret_cast< glm::mat4& >( cmd.Common.mvp ) = vp;

		cmd.Common.func( &cmd );
	}

	SDL_GL_SwapWindow( sdl_window );
}

static void tick()
{
	u32 dt = SDL_GetTicks() - t0;
	t0 += dt;
	handle_input( dt );
	render( dt );

#ifdef __EMSCRIPTEN__
	if ( !g_stay_alive )
	{
		g_stay_alive = true; // Do this before cancel because cancel invokes this function again!
		cout << "emscripten cancel main loop and restart" << endl;
		emscripten_cancel_main_loop();
		//emscripten_set_main_loop( tick, 0, 1 );
	}
#endif
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

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CW );

	// fragment blending
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// depth
	glDisable( GL_DEPTH_TEST );

	glClearColor( 1.0f, 0.1f, 1.0f, 1.0f );

	{
		ShaderLoader_OpenGL shader_loader;
		u32 shader_handle;
		shader_loader.load( (shader_source_prefix+vert).c_str(), (shader_source_prefix+frag).c_str(), shader_handle );

		MeshLoader_OpenGL mesh_loader;
		Mesh quad_mesh = mesh_loader.load( sizeof(quad), quad );

		quad_key.RenderTranslucent.translucency_type = 1;
		quad_key.RenderTranslucent.depth = 1;

		quad_cmd.DrawBitmap.func = draw_bitmap;
		reinterpret_cast< glm::mat4& >( quad_cmd.DrawBitmap.mvp ) = glm::mat4( 1.f );
		quad_cmd.DrawBitmap.vbo_n = quad_mesh.vbo_n;
		quad_cmd.DrawBitmap.vbo = quad_mesh.vbo;
		quad_cmd.DrawBitmap.shader = shader_handle;
//	quad_cmd.DrawBitmap.bitmap = reinterpret_cast< RetroGraphics::TextureManager_OpenGL& >( *_texture_manager ).lookup( bitmap );
		quad_cmd.DrawBitmap.size[ 0 ] = 128;
		quad_cmd.DrawBitmap.size[ 1 ] = 128;

		render_queue.cmd.push_back( quad_cmd );
		render_queue.key.push_back( quad_key );
		render_queue.key[ render_queue.size ].RenderCommon.data_index = render_queue.size;
		++render_queue.size;
	}

	{
		int w, h;
		SDL_GL_GetDrawableSize( sdl_window, &w, &h );
		float ar = (float)w / h;
		float z_near = .1f;
		float z_far = 1000.f;

		proj = glm::perspective( glm::radians(70.f), ar, z_near, z_far );
		pos = glm::vec3( 0.f, 0.f, 2.f );
	}

	t0 = SDL_GetTicks();

#ifdef __EMSCRIPTEN__
	// TODO Use emscripten_set_main_loop_arg for passing args!
	// Example: emscripten_set_main_loop_arg((em_arg_callback_func)loop_iteration, game.get(), 0, 1);
	emscripten_set_main_loop( tick, 0, 1 );
#else
	while ( g_stay_alive )
	{
		tick();
	}
#endif

	SDL_Quit();

	cout << "Shutdown" << endl;
	return ReturnStatus::OK;
}
