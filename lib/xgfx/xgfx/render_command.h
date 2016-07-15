#pragma once

#include "headers_type.h"
#include "render_func.h"
#include "render_data.h"

namespace xgfx
{
	struct RenderCommand
	{
		RenderFunc func;
		RenderData data;
	};
}
