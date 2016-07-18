#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <xgfx/headers_gl.h>
#include <xgfx/render_queue.h>
#include <xecs/system.h>

#include "component_camera.h"

namespace
{
	inline bool compare_render_key( xgfx::RenderKey a, xgfx::RenderKey b )
	{
		return a.raw < b.raw;
	}
}

struct RenderContext
{
	glm::mat4 v;
	glm::mat4 p;
	glm::mat4 vp;
};

class SystemRender : public xecs::System
{
	SDL_Window*                        _sdl_window;
	std::shared_ptr<xgfx::RenderQueue> _render_queue;
	std::shared_ptr<ComponentCamera >  _c_cam;

	RenderContext _render_context;

public:
	explicit SystemRender(
			SDL_Window* sdl_window,
			std::shared_ptr<xgfx::RenderQueue> render_queue,
			std::shared_ptr<ComponentCamera> c_cam )
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
		_render_context.v = _c_cam->_data.view[ 0 ]; // TODO Ugly hack for using first camera!
		_render_context.p = _c_cam->_data.proj[ 0 ]; // TODO Ugly hack for using first camera!
		_render_context.vp = _c_cam->_data.proj[ 0 ] * _c_cam->_data.view[ 0 ]; // TODO Ugly hack for using first camera!

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		std::sort( _render_queue->key.data(), _render_queue->key.data() + _render_queue->size, compare_render_key );
		for ( u32 i = 0, n = _render_queue->size; i < n; ++i )
		{
			int command_i = _render_queue->key[ i ].RenderCommon.data_index;
			xgfx::RenderCommand& cmd = _render_queue->cmd[ command_i ];
			cmd.func( &_render_context, &cmd.data );
		}

		SDL_GL_SwapWindow( _sdl_window );
	}
};
