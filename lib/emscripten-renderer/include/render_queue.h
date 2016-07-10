#pragma once

#include <vector>

#include "headers_type.h"
#include "render_key.h"
#include "render_command.h"

namespace Raphael
{
	struct RenderQueue
	{
		u32                          size = 0;
		std::vector< RenderKey >     key;
		std::vector< RenderCommand > cmd;
	};
}
