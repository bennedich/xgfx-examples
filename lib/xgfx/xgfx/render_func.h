#pragma once

namespace xgfx
{
	union RenderCommand;
	typedef void ( *RenderFunc )( const RenderCommand* );
}
