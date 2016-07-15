#include <cstdint>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>

#include <xgfx/headers_type.h>
#include <xgfx/headers_gl.h>

#include <xgfx/shader_loader.h>
#include <xgfx/mesh_loader.h>

#include <xgfx/render_key.h>
#include <xgfx/render_command.h>
#include <xgfx/render_queue.h>
#include <xgfx/render_func.h>

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

using namespace xgfx;

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

	union RenderDataBitmap
	{
		RenderData raw;
		struct {
			u32        vbo_n;
			u32        vbo; // vbo contains attributes such as vert,norm,color, these must then also exist in shader.
			u32        shader;
			u32        bitmap;
			float      size[ 2 ];
		};
	};
	static_assert( sizeof(RenderDataBitmap) == xgfx::RENDER_DATA_SIZE, "RenderData wrong size." );

	RenderKey     quad_key;
	RenderCommand quad_cmd;
	RenderQueue   render_queue;

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
			gl_Position = vec4( pos, 1.0 );//mvp * vec4( pos, 1.0 );
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

	void draw_bitmap( const void*, const RenderDataBitmap* data )
	{
		glUseProgram( data->shader );

//		glUniformMatrix4fv( glGetUniformLocation( command->DrawBitmap.shader, "mvp" ), 1, GL_FALSE, command->DrawBitmap.mvp );
//		glUniform2fv( glGetUniformLocation( command->DrawBitmap.shader, "size" ), 1, command->DrawBitmap.size );
//
		GLsizei stride = sizeof( VertexBitmap );
		int n_verts = data->vbo_n / stride;
		auto uv_offset = (const void*)sizeof( VertexBitmap::pos );

		glBindBuffer( GL_ARRAY_BUFFER, data->vbo );
		glVertexAttribPointer( glGetAttribLocation( data->shader, "pos" ), 3, GL_FLOAT, GL_FALSE, stride, 0 );
		glVertexAttribPointer( glGetAttribLocation( data->shader, "uv" ), 2, GL_FLOAT, GL_FALSE, stride, uv_offset );
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

static void handle_events()
{
	SDL_Event e;
	while ( SDL_PollEvent(&e) )
	{
		switch( e.type )
		{
			case SDL_QUIT:
				g_stay_alive = false;
				break;
			case SDL_KEYDOWN:
				if ( e.key.keysym.sym == SDLK_q )
					g_stay_alive = false;
				else
					cout << "Unhandled key." << endl;
				break;
		}
	}
}

static void render()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	std::sort( render_queue.key.data(), render_queue.key.data() + render_queue.size, compare_render_key );
	for ( u32 i = 0, n = render_queue.size; i < n; ++i )
	{
		int command_i = render_queue.key[ i ].RenderCommon.data_index;
		RenderCommand& cmd = render_queue.cmd[ command_i ];
		cmd.func( 0, &cmd.data );
	}

	SDL_GL_SwapWindow( sdl_window );
}

static void tick()
{
	handle_events();
	render();

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
		"xgfx",
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

		RenderDataBitmap data;
		data.vbo_n = quad_mesh.vbo_n;
		data.vbo = quad_mesh.vbo;
		data.shader = shader_handle;
	//	data.bitmap = reinterpret_cast< RetroGraphics::TextureManager_OpenGL& >( *_texture_manager ).lookup( bitmap );
		data.size[ 0 ] = 128;
		data.size[ 1 ] = 128;

		quad_cmd.func = (xgfx::RenderFunc) draw_bitmap;
		quad_cmd.data = data.raw;

		render_queue.cmd.push_back( quad_cmd );
		render_queue.key.push_back( quad_key );
		render_queue.key[ render_queue.size ].RenderCommon.data_index = render_queue.size;
		++render_queue.size;
	}

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
