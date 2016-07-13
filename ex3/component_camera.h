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

class ComponentCamera : public xecs::Component< ComponentCamera >
{
public:
	struct InstanceData
	{
		vector< Entity > entity;
		vector< mat4 >   view;
		vector< mat4 >   proj;
	} _data;

	ComponentCamera( u32 capacity ) : Component( capacity )
	{
		_data.entity.resize( capacity );
		_data.view.resize( capacity );
		_data.proj.resize( capacity );
	}

	void _copy( unsigned i, unsigned i_dst, ComponentCamera::InstanceData& data_dst )
	{
		data_dst.entity[ i_dst ] = _data.entity.at( i );
		data_dst.view[ i_dst ]   = _data.view.at( i );
		data_dst.proj[ i_dst ]   = _data.proj.at( i );
	}

	inline mat4& view( Entity e ) { return get( _data.view, e ); }
	inline void set_view( Entity e, mat4& view ) { set( _data.view, e, view ); }

	inline mat4& proj( Entity e ) { return get( _data.proj, e ); }
	inline void set_proj( Entity e, mat4& proj ) { set( _data.proj, e, proj ); }
};
