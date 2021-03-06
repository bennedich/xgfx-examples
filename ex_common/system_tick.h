#pragma once

#include <memory>
#include <SDL.h>
#include <xecs/system.h>
#include "time_delta.h"

class SystemTick : public xecs::System
{
	std::shared_ptr< TimeDelta > _time;

public:
	explicit SystemTick( std::shared_ptr<TimeDelta> time )
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
