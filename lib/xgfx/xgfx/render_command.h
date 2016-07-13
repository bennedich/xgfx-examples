#pragma once

#include "headers_type.h"
#include "render_func.h"

namespace xgfx
{
	union RenderCommand
	{
		struct
		{
			RenderFunc func;
			float      mvp[ 16 ];
		} Common;

		struct
		{
			RenderFunc func;
			float      mvp[ 16 ];
			u32        vbo_n;
			u32        vbo; // vbo contains attributes such as vert,norm,color, these must then also exist in shader.
			u32        shader;
			u32        bitmap;
			float      size[ 2 ];
		} DrawBitmap;

		struct
		{
			RenderFunc func;
			float      mvp[ 16 ];
			u32        vbo_n;
			u32        vbo; // vbo contains attributes such as vert,norm,color, these must then also exist in shader.
			u32        shader;
			u32        bitmap;
			float      font_size;
//			RetroResource::Handle mesh_handle;
		} DrawString;
	};
}
