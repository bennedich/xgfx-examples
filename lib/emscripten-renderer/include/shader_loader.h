#pragma once

#include "platform.h"

namespace Raphael
{
	class IShaderLoader
	{
	public:
		virtual ~IShaderLoader() {}
		virtual bool load( const char* vert, const char* frag, u32& prog_handle ) const = 0;
		virtual void unload( const u32 prog_handle ) const = 0;
	};


	class ShaderLoader_OpenGL : public IShaderLoader
	{
	public:
		virtual bool load( const char* vert, const char* frag, GLuint& prog_handle ) const override;
		virtual void unload( const GLuint prog_handle ) const override;
	};
}
