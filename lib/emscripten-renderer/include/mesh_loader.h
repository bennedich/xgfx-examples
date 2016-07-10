#pragma once

#include "headers_type.h"
#include "mesh.h"

namespace Raphael
{
	class IMeshLoader
	{
	public:
		virtual ~IMeshLoader() {}
		virtual Mesh load( const u32 vertices_n, const void* vertices, const u32 indices_n = 0, const void* indices = 0 ) const = 0;
		virtual void unload( const Mesh& mesh ) const = 0;
	};


	class MeshLoader_OpenGL : public IMeshLoader
	{
	public:
		virtual Mesh load( const u32 vertices_n, const void* vertices, const u32 indices_n = 0, const void* indices = 0 ) const override;
		virtual void unload( const Mesh& mesh ) const override;
	};
}
