#pragma once

#include "xecs/type.h"
#include "xecs/entity_aux.h"

namespace xecs
{
	struct Entity
	{
		u32 id;
		inline u32 index() const { return id & entity_aux::INDEX_MASK; }
		inline u32 generation() const { return ( id >> entity_aux::INDEX_BITS ) & entity_aux::GENERATION_MASK; }
	};
}
