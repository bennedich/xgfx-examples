#pragma once

#include <SDL.h>

#include <xecs_system.h>

#include "component_transform.h"
#include "component_camera.h"

class SystemCamera : public xecs::System
{
	SDL_Window*                          _sdl_window;
	std::shared_ptr<ComponentTransform > _c_tfm;
	std::shared_ptr<ComponentCamera >    _c_cam;

public:
	explicit SystemCamera( SDL_Window* sdl_window, std::shared_ptr<ComponentTransform> c_tfm, std::shared_ptr<ComponentCamera> c_cam )
		: _sdl_window( sdl_window )
		, _c_tfm( c_tfm )
		, _c_cam( c_cam )
	{
	}

	virtual void tick() override
	{
		int w, h;
		SDL_GL_GetDrawableSize( _sdl_window, &w, &h );
		float ar = (float)w / h;
		float z_near = .1f;
		float z_far = 1000.f;

		for ( u32 i = 0, n = _c_cam->size(); i < n; ++i )
		{
			auto e = _c_cam->_data.entity[ i ];

			_c_cam->_data.proj[ i ] = glm::perspective( glm::radians(70.f), ar, z_near, z_far );
			_c_cam->_data.view[ i ] = glm::inverse( _c_tfm->transform(e) );
		}
	}
};
