#pragma once

#include <xecs/component.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

using std::vector;
using glm::vec3;
using glm::quat;
using glm::mat4;
using xecs::Entity;

class ComponentPhysics : public xecs::Component< ComponentPhysics >
{
public:
	struct InstanceData
	{
		vector< Entity > entity;
		vector< vec3 >   posv;
		vector< quat >   rotv;
	} _data;

	ComponentPhysics( u32 capacity ) : Component( capacity )
	{
		_data.entity.resize( capacity );
		_data.posv.resize( capacity );
		_data.rotv.resize( capacity );
	}

	void _copy( unsigned i, unsigned i_dst, ComponentPhysics::InstanceData& data_dst )
	{
		data_dst.entity[ i_dst ] = _data.entity.at( i );
		data_dst.posv[ i_dst ]   = _data.posv.at( i );
		data_dst.rotv[ i_dst ]   = _data.rotv.at( i );
	}

	inline vec3& posv( Entity e ) { return get( _data.posv, e ); }
	inline void set_posv( Entity e, const vec3& posv ) { set( _data.posv, e, posv ); }

	inline quat& rotv( Entity e ) { return get( _data.rotv, e ); }
	inline void set_rotv( Entity e, quat& rotv ) { set( _data.rotv, e, rotv ); }
};
