#pragma once

#define RENDER_INPUT_BITS_DATA_INDEX 13
#define RENDER_INPUT_BITS_DEPTH 8

#define RENDER_INPUT_ARRAY_SIZE 1 << RENDER_INPUT_BITS_DATA_INDEX

#define RENDER_INPUT_DATA_INDEX        u32 data_index        : RENDER_INPUT_BITS_DATA_INDEX;
#define RENDER_INPUT_MATERIAL_INDEX    u32 material_index    : 8;
#define RENDER_INPUT_DEPTH             u32 depth             : RENDER_INPUT_BITS_DEPTH;
#define RENDER_INPUT_IS_COMMAND        u32 is_command        : 1;
#define RENDER_INPUT_TRANSLUCENCY_TYPE u32 translucency_type : 2;

#include "headers_type.h"

namespace Raphael
{
	union RenderKey
	{
		u32 raw;

		struct
		{
			RENDER_INPUT_DATA_INDEX
			u32 padding : 16;
			RENDER_INPUT_IS_COMMAND
			RENDER_INPUT_TRANSLUCENCY_TYPE
		} RenderCommon;

		struct
		{
			RENDER_INPUT_DATA_INDEX
			RENDER_INPUT_DEPTH
			RENDER_INPUT_MATERIAL_INDEX
			RENDER_INPUT_IS_COMMAND
			RENDER_INPUT_TRANSLUCENCY_TYPE
		} RenderOpaque;

		struct
		{
			RENDER_INPUT_DATA_INDEX
			RENDER_INPUT_MATERIAL_INDEX
			RENDER_INPUT_DEPTH
			RENDER_INPUT_IS_COMMAND
			RENDER_INPUT_TRANSLUCENCY_TYPE
		} RenderTranslucent;

		struct
		{
			RENDER_INPUT_DATA_INDEX
			u32 command : 10;
			u32 sequence : 6;
			RENDER_INPUT_IS_COMMAND
			RENDER_INPUT_TRANSLUCENCY_TYPE
		} RenderCommand;
	};
}
