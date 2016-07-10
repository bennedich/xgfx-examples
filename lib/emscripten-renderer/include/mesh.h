#pragma once

#include "headers_type.h"

namespace Raphael
{
	struct Mesh
	{
		union
		{
			u32 raw[ 4 ];
			struct { u32 vbo_n, ibo_n, vbo, ibo; };
		};
	};
}