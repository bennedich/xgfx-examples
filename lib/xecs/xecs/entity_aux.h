#pragma once

#include "xecs/type.h"

namespace xecs
{
	namespace entity_aux
	{
		const u32 INDEX_BITS = 22;
		const u32 GENERATION_BITS = 8;
		const u32 INDEX_MASK = ( 1 << INDEX_BITS ) - 1;
		const u32 GENERATION_MASK = ( 1 << GENERATION_BITS ) - 1;
		const u32 MINIMUM_FREE_INDICES = 1024;
		const u32 INVALID_HANDLE = 0;
	}
}
