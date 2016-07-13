#pragma once

#include <vector>
#include <deque>

#include "xecs/type.h"
#include "xecs/entity.h"

namespace xecs
{
	class EntityManager
	{
		std::vector<unsigned char> _generation;
		std::deque<unsigned>       _free_indices;

	public:
		EntityManager()
		{
			_generation.reserve( entity_aux::MINIMUM_FREE_INDICES );
			create(); // Reserve Entity.id == 0 for invalid entities.
		}

		inline bool is_alive( Entity e ) const
		{
			return _generation[ e.index() ] == e.generation();
		}

		Entity create()
		{
			unsigned idx;
			if ( _free_indices.size() > entity_aux::MINIMUM_FREE_INDICES )
			{
				idx = _free_indices.front();
				_free_indices.pop_front();
			}
			else
			{
				_generation.push_back( 0 );
				idx = _generation.size() - 1;
			}
			return Entity { idx + (_generation[ idx ] << entity_aux::INDEX_BITS) };
		}

		void destroy( Entity e )
		{
			const unsigned idx = e.index();
			++_generation[ idx ];
			_free_indices.push_back( idx );
		}
	};
}
