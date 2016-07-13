#pragma once

#include <headers_gl.h>   // TODO Move this into a namespace.
#include <xecs_system.h>

#include "render_queue.h"
#include "component_camera.h"

using namespace Raphael;

namespace
{
	inline bool compare_render_key ( RenderKey a, RenderKey b )
	{
		return a.raw < b.raw;
	}
}

class SystemRender : public xecs::System
{
	SDL_Window*                       _sdl_window;
	std::shared_ptr<RenderQueue>      _render_queue;
	std::shared_ptr<ComponentCamera > _c_cam;

public:
	explicit SystemRender( SDL_Window* sdl_window, std::shared_ptr<RenderQueue> render_queue, std::shared_ptr<ComponentCamera> c_cam )
		: _sdl_window( sdl_window )
		, _render_queue( render_queue )
		, _c_cam( c_cam )
	{
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		glFrontFace( GL_CW );

		// fragment blending
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// depth
		glDisable( GL_DEPTH_TEST );

		glClearColor( 1.0f, 0.1f, 1.0f, 1.0f );
	}

	virtual void tick() override
	{
		glm::mat4 vp = _c_cam->_data.proj[ 0 ] * _c_cam->_data.view[ 0 ]; // TODO Ugly hack for using first camera!


		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		std::sort( _render_queue->key.data(), _render_queue->key.data() + _render_queue->size, compare_render_key );
		for ( u32 i = 0, n = _render_queue->size; i < n; ++i )
		{
			int command_i = _render_queue->key[ i ].RenderCommon.data_index;
			RenderCommand& cmd = _render_queue->cmd[ command_i ];

			reinterpret_cast< glm::mat4& >( cmd.Common.mvp ) = vp;

			cmd.Common.func( &cmd );
		}

		SDL_GL_SwapWindow( _sdl_window );
	}
};
