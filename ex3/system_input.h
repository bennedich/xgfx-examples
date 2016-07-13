#pragma once

#include <string> // std::memset

#include <xecs_system.h>
#include "input_context.h"

class SystemInput : public xecs::System
{
	std::shared_ptr<bool>         _stay_alive;
	std::shared_ptr<InputContext> _input_context;

public:
	explicit SystemInput( std::shared_ptr<bool> stay_alive, std::shared_ptr<InputContext> input_context )
		: _stay_alive( stay_alive )
		, _input_context( input_context )
	{
	}

	virtual void tick() override
	{
		update_input_context();
		handle_input_events();
	}

private:
	void update_input_context()
	{
		std::memset( &*_input_context, 0, sizeof(*_input_context) );

		// Translate.

		const uint8_t* state = SDL_GetKeyboardState( NULL );
		if ( state[SDL_SCANCODE_A] )
			_input_context->move_x -= 1.f;
		if ( state[SDL_SCANCODE_D] )
			_input_context->move_x += 1.f;
		if ( state[SDL_SCANCODE_F] )
			_input_context->move_y -= 1.f;
		if ( state[SDL_SCANCODE_R] )
			_input_context->move_y += 1.f;
		if ( state[SDL_SCANCODE_S] )
			_input_context->move_z += 1.f;
		if ( state[SDL_SCANCODE_W] )
			_input_context->move_z -= 1.f;

		// Rotate.

		if ( state[SDL_SCANCODE_E] )
			_input_context->rotate_z -= 1.f;
		if ( state[SDL_SCANCODE_Q] )
			_input_context->rotate_z += 1.f;
		if ( state[SDL_SCANCODE_RIGHT] )
			_input_context->rotate_y -= 1.f;
		if ( state[SDL_SCANCODE_LEFT] )
			_input_context->rotate_y += 1.f;
		if ( state[SDL_SCANCODE_DOWN] )
			_input_context->rotate_x -= 1.f;
		if ( state[SDL_SCANCODE_UP] )
			_input_context->rotate_x += 1.f;
	}

	void handle_input_events()
	{
		SDL_Event e;
		while ( SDL_PollEvent(&e) )
		{
			switch( e.type )
			{
				case SDL_QUIT:
					*_stay_alive = false;
					break;
				case SDL_KEYDOWN:
					switch( e.key.keysym.sym )
					{
						case SDLK_ESCAPE:
							*_stay_alive = false;
							break;
					}
					break;
			}
		}
	}
};