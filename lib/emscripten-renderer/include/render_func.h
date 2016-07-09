#pragma once

namespace Raphael
{
	union RenderCommand;
	typedef void ( *RenderFunc )( const RenderCommand* );
}
