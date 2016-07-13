#pragma once

#include <memory>
#include <vector>

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#include "xecs/system.h"

namespace xecs
{
	class Application
	{
		std::shared_ptr<bool> _stay_alive;
		std::vector< std::shared_ptr<System> > _systems;

		inline void tick()
		{
			for ( auto system : _systems )
			{
				system->tick();
			}
		}

	public:
		explicit Application( std::shared_ptr<bool> stay_alive, std::vector< std::shared_ptr<System> > systems )
			: _stay_alive( stay_alive )
			, _systems( systems )
		{}

#ifdef __EMSCRIPTEN__
		void run()
		{
			emscripten_set_main_loop_arg( [](void* app)
			{
				((Application*)app)->tick();

				// TODO Handle emscripten_cancel_main_loop ??

			}, this, 0, 1 );
		}
#else
		void run()
		{
			*_stay_alive = true;
			while ( *_stay_alive )
			{
				tick();
			}
		}
#endif
	};
}