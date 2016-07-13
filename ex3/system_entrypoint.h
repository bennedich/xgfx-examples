#pragma once

#include <xecs_system.h>

#include <render_key.h>
#include <render_command.h>
#include <render_queue.h>
#include <render_func.h>

#include <shader_loader.h>
#include <mesh_loader.h>

#include <xecs_entity_manager.h>

#include "render_funcs.h"
#include "component_transform.h"
#include "component_camera.h"

namespace
{
	Raphael::RenderKey     quad_key;
	Raphael::RenderCommand quad_cmd;

	VertexBitmap quad_mesh_raw[ 4 ] = {
		{ -.5f, -.5f, .0f, 0.f, 0.f },
		{ -.5f,  .5f, .0f, 0.f, 1.f },
		{  .5f, -.5f, .0f, 1.f, 0.f },
		{  .5f,  .5f, .0f, 1.f, 1.f },
	};

#ifdef __EMSCRIPTEN__
	const std::string shader_head = R"(
		precision mediump float;
		)";
#else
	const std::string shader_head = R"(
		)";
#endif

	const std::string vert = R"(
		uniform mat4 mvp;

		attribute vec3 pos;
		//attribute vec4 col;
		attribute vec2 uv;

		varying vec4 f_color;
		varying vec2 f_uv;

		void main ()
		{
			gl_Position = mvp * vec4( pos, 1.0 );
			f_color = vec4(0, uv, 1);//col;
			f_uv = uv;
		}
	)";

	const std::string frag = R"(
		//uniform sampler2D texture;

		varying vec4 f_color;
		varying vec2 f_uv;

		void main ()
		{
			gl_FragColor = f_color;// * texture2D( texture, f_uv );
		}
	)";
}

class SystemEntrypoint : public xecs::System
{
	std::shared_ptr< RenderFuncs >          _render_funcs;
	std::shared_ptr< Raphael::RenderQueue > _render_queue;
	std::shared_ptr< xecs::EntityManager >  _e_mgr;
	std::shared_ptr< ComponentTransform >   _c_tfm;
	std::shared_ptr< ComponentPhysics >     _c_phy;
	std::shared_ptr< ComponentCamera >      _c_cam;

	xecs::Entity e_cam;
	xecs::Entity e_quad;

public:
	explicit SystemEntrypoint(
			std::shared_ptr< RenderFuncs > render_funcs,
			std::shared_ptr< Raphael::RenderQueue > render_queue,
			std::shared_ptr< xecs::EntityManager > e_mgr,
			std::shared_ptr< ComponentTransform > c_tfm,
			std::shared_ptr< ComponentPhysics > c_phy,
			std::shared_ptr< ComponentCamera > c_cam )
		: _render_funcs( render_funcs )
		, _render_queue( render_queue )
		, _e_mgr( e_mgr )
		, _c_tfm( c_tfm )
		, _c_phy( c_phy )
		, _c_cam( c_cam )
	{
		// Add camera.
		{
			e_cam = _e_mgr->create();
			auto i_cam = _c_cam->create( e_cam );
			auto i_phy = _c_phy->create( e_cam );
			_c_phy->_data.posv[ i_phy ] = glm::vec3();
			_c_phy->_data.rotv[ i_phy ] = glm::quat();
			auto i_tfm = _c_tfm->create( e_cam );
			_c_tfm->_data.pos[ i_tfm ] = glm::vec3( 0.f, 0.f, 2.f );
			_c_tfm->_data.rot[ i_tfm ] = glm::quat();
			_c_tfm->_data.scale[ i_tfm ] = 1.f;
		}

		// Add quad.
		{
			e_quad = _e_mgr->create();
			auto i_tfm = _c_tfm->create( e_quad );
			_c_tfm->_data.pos[ i_tfm ] = glm::vec3();
			_c_tfm->_data.rot[ i_tfm ] = glm::quat();
			_c_tfm->_data.scale[ i_tfm ] = 1.f;

			Raphael::ShaderLoader_OpenGL shader_loader;
			u32 shader_handle;
			shader_loader.load( (shader_head+vert).c_str(), (shader_head+frag).c_str(), shader_handle );

			Raphael::MeshLoader_OpenGL mesh_loader;
			Raphael::Mesh quad_mesh = mesh_loader.load( sizeof(quad_mesh_raw), quad_mesh_raw );

			quad_key.RenderTranslucent.translucency_type = 1;
			quad_key.RenderTranslucent.depth = 1;

			quad_cmd.DrawBitmap.func = _render_funcs->draw_bitmap;
			reinterpret_cast< glm::mat4& >( quad_cmd.DrawBitmap.mvp ) = glm::mat4( 1.f );
			quad_cmd.DrawBitmap.vbo_n = quad_mesh.vbo_n;
			quad_cmd.DrawBitmap.vbo = quad_mesh.vbo;
			quad_cmd.DrawBitmap.shader = shader_handle;
//	quad_cmd.DrawBitmap.bitmap = reinterpret_cast< RetroGraphics::TextureManager_OpenGL& >( *_texture_manager ).lookup( bitmap );
			quad_cmd.DrawBitmap.size[ 0 ] = 128;
			quad_cmd.DrawBitmap.size[ 1 ] = 128;

			_render_queue->cmd.push_back( quad_cmd );
			_render_queue->key.push_back( quad_key );
			_render_queue->key[ _render_queue->size ].RenderCommon.data_index = _render_queue->size;
			++_render_queue->size;
		}
	}

	virtual ~SystemEntrypoint() override
	{
		// TODO Delete entities and resources!
	}

//	virtual void tick() override
//	{
//	}
};