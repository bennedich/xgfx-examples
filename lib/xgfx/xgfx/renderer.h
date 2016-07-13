//#pragma once
//
//#include <algorithm>
//#include <memory>
//
//#include "platform.h"
//#include "render_key.h"
//#include "render_command.h"
//#include "render_func.h"
//
//namespace
//{
//	inline bool compare_render_key ( xgfx::RenderKey a, xgfx::RenderKey b )
//	{
//		return a.raw < b.raw;
//	}
//}
//
//namespace xgfx
//{
//	class Renderer
//	{
//// TODO		shared_ptr< RetroEnvironment::IManager > _env;
//		u32                                      _render_count;
//		RenderKey                                _render_key[ 1024 ];
//		RenderCommand                            _render_command[ 1024 ];
//
//	public:
//		explicit Renderer()// shared_ptr< RetroEnvironment::IManager > env )
//			: _render_count( 0 )
//// TODO		explicit Renderer( shared_ptr< RetroEnvironment::IManager > env )
//// TODO			: _env( env )
//// TODO			, _render_count( 0 )
//		{}
//
//		RenderCommand & submit( const RenderKey key )
//		{
//			_render_key[ _render_count ] = key;
//			_render_key[ _render_count ].RenderCommon.data_index = _render_count;
//			RenderCommand & command = _render_command[ _render_count ];
//			++_render_count;
//			return command;
//		}
//
//		void render()
//		{
//			std::sort( _render_key, _render_key + _render_count, compare_render_key );
//			for ( u32 i = 0; i < _render_count; ++i )
//			{
//				int command_i = _render_key[ i ].RenderCommon.data_index;
//				RenderCommand& command = _render_command[ command_i ];
//// TODO				command.Common.func( &*_env, &command );
//				command.Common.func( &command );
//			}
//			_render_count = 0;
//		}
//	};
//}
