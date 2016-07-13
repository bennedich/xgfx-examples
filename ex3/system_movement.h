#pragma once

#include <memory>
#include <algorithm> // std::min

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>   // glm::translate, glm::rotate, glm::scale

#include <xecs/entity.h>
#include <xecs/system.h>

#include "input_context.h"
#include "component_transform.h"
#include "component_physics.h"
#include "component_camera.h"

class SystemMovement : public xecs::System
{
	std::shared_ptr<const Time>         _time;
	std::shared_ptr<const InputContext> _input_context;
	std::shared_ptr<ComponentTransform> _c_tfm;
	std::shared_ptr<ComponentPhysics>   _c_phy;
	std::shared_ptr<ComponentCamera>    _c_cam;

	const float D_POS = .02f; // Delta movement.
	const float D_ROT = .01f; // Delta rotation.
	const float RETARDATION = .12f;
	const glm::vec3 VEC3_ZERO;
	const glm::quat QUAT_ZERO;

	u32 TIME_STEP = 16u; // Milliseconds.
	u32 time_pool;

public:
	explicit SystemMovement(
			std::shared_ptr<const Time> time,
			std::shared_ptr<const InputContext> input_context,
			std::shared_ptr<ComponentTransform> c_tfm,
			std::shared_ptr<ComponentPhysics> c_phy,
			std::shared_ptr<ComponentCamera> c_cam )
		: _time( time )
		, _input_context( input_context )
		, _c_tfm( c_tfm )
		, _c_phy( c_phy )
		, _c_cam( c_cam )
	{

	}

	virtual void tick() override
	{
		// TODO Fixed timestep but with occasional stuttering.
		// TODO The visualization transforms should be lerped with the remaining time_pool/TIME_STEP.

		time_pool = std::min( 1000u, time_pool + _time->dt );
		while ( time_pool > TIME_STEP )
		{
			time_pool -= TIME_STEP;

			fixed_tick_camera_movement();
			fixed_tick_physics();
		}

		sync_transforms();
	}

private:
	void sync_transforms()
	{
		// Update all model mat4 transforms from their SRT components.

		auto& tfm = _c_tfm->_data.transform;
		auto& pos = _c_tfm->_data.pos;
		auto& rot = _c_tfm->_data.rot;
		// TODO auto& scale = _c_tfm->_data.scale;

		for ( u32 i = 0, n = _c_tfm->size(); i < n; ++i )
		{
			tfm[ i ] = glm::translate( glm::mat4(1), pos[ i ] ) * glm::mat4_cast( rot[ i ] ); // TODO * scale ??
		}
	}

	void fixed_tick_physics()
	{
		auto& entity = _c_phy->_data.entity;
		auto& posv = _c_phy->_data.posv;
		auto& rotv = _c_phy->_data.rotv;
		auto& pos = _c_tfm->_data.pos;
		auto& rot = _c_tfm->_data.rot;

		for ( u32 i = 0, n = _c_phy->size(); i < n; ++i )
		{
			u32 i_tfm = _c_tfm->lookup( entity[i] );
			pos[ i_tfm ] = posv[ i ] + pos[ i_tfm ];
			rot[ i_tfm ] = glm::normalize( rotv[i] * rot[i_tfm] );
		}
	}

	void fixed_tick_camera_movement()
	{
		xecs::Entity e_cam = _c_cam->_data.entity[ 0 ]; // TODO Ugly hack for using first camera!
		u32 i_tfm = _c_tfm->lookup( e_cam );
		u32 i_phy = _c_phy->lookup( e_cam );

		// Extract transform components right, up, forward.

		glm::mat4& tfm = _c_tfm->_data.transform[ i_tfm ];
		glm::vec3& r = reinterpret_cast< glm::vec3& >( tfm[ 0 ] );
		glm::vec3& u = reinterpret_cast< glm::vec3& >( tfm[ 1 ] );
		glm::vec3& f = reinterpret_cast< glm::vec3& >( tfm[ 2 ] );

		// Translate.
		auto& posv = _c_phy->_data.posv[ i_phy ];

		if ( _input_context->move_x )
			posv += (_input_context->move_x * D_POS) * r;
		if ( _input_context->move_y )
			posv += (_input_context->move_y * D_POS) * u;
		if ( _input_context->move_z )
			posv += (_input_context->move_z * D_POS) * f;

		posv = glm::mix( posv, VEC3_ZERO, RETARDATION );

		// Rotate.
		auto& rotv = _c_phy->_data.rotv[ i_phy ];

		if ( _input_context->rotate_z )
			rotv = glm::angleAxis( _input_context->rotate_z * D_ROT, f ) * rotv;
		if ( _input_context->rotate_y )
			rotv = glm::angleAxis( _input_context->rotate_y * D_ROT, u ) * rotv;
		if ( _input_context->rotate_x )
			rotv = glm::angleAxis( _input_context->rotate_x * D_ROT, r ) * rotv;

		rotv = glm::lerp( rotv, QUAT_ZERO, RETARDATION );
	}
};