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

class ComponentTransform : public xecs::Component< ComponentTransform >
{
public:
	struct InstanceData
	{
		vector< Entity > entity;
		vector< vec3 >   pos;
		vector< quat >   rot;
		vector< float >  scale;
		vector< mat4 >   transform;
	} _data;

	ComponentTransform( u32 capacity ) : Component( capacity )
	{
		_data.entity.resize( capacity );
		_data.pos.resize( capacity );
		_data.rot.resize( capacity );
		_data.scale.resize( capacity );
		_data.transform.resize( capacity );
	}

	void _copy( unsigned i, unsigned i_dst, ComponentTransform::InstanceData& data_dst )
	{
		data_dst.entity[ i_dst ] = _data.entity.at( i );
		data_dst.pos[ i_dst ] = _data.pos.at( i );
		data_dst.rot[ i_dst ] = _data.rot.at( i );
		data_dst.scale[ i_dst ] = _data.scale.at( i );
		data_dst.transform[ i_dst ] = _data.transform.at( i );
	}

	inline vec3& pos( Entity e ) { return get( _data.pos, e ); }
	inline void set_pos( Entity e, const vec3& pos ) { set( _data.pos, e, pos ); }

	inline quat& rot( Entity e ) { return get( _data.rot, e ); }
	inline void set_rot( Entity e, quat& rot ) { set( _data.rot, e, rot ); }

	inline float scale( Entity e ) { return get( _data.scale, e ); }
	inline void set_scale( Entity e, float scale ) { set( _data.scale, e, scale ); }

	inline mat4& transform( Entity e ) { return get( _data.transform, e ); }
	inline void set_transform( Entity e, mat4& transform ) { set( _data.transform, e, transform ); }
};
