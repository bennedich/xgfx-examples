#pragma once

#include <memory>
#include <SDL.h>
#include <xecs_system.h>
#include "time.h"

class SystemTick : public xecs::System
{
	std::shared_ptr< Time > _time;

public:
	explicit SystemTick( std::shared_ptr<Time> time )
		: _time( time )
	{
		_time->t0 = SDL_GetTicks() - 17;
	}

	virtual void tick() override
	{
		_time->t0 = _time->t1;
		_time->t1 = SDL_GetTicks();
		_time->dt = _time->t1 - _time->t0;
	}
};
