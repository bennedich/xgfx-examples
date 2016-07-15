#pragma once

#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr

#include <xgfx/render_func.h>
#include <xgfx/render_command.h>

struct VertexBitmap
{
	float pos[ 3 ];
	float uv[ 2 ];
};

union RenderDataBitmap
{
	xgfx::RenderData raw;
	struct {
		u32        vbo_n;
		u32        vbo; // vbo contains attributes such as vert,norm,color, these must then also exist in shader.
		u32        shader;
		u32        bitmap;
		float      size[ 2 ];
	};
};
static_assert( sizeof(RenderDataBitmap) == xgfx::RENDER_DATA_SIZE, "RenderData wrong size." );

void draw_bitmap( const RenderContext* ctx, const RenderDataBitmap* data )
{
	glUseProgram( data->shader );

	glUniformMatrix4fv( glGetUniformLocation( data->shader, "mvp" ), 1, GL_FALSE, glm::value_ptr(ctx->vp) );
//	glUniform2fv( glGetUniformLocation( command->DrawBitmap.shader, "size" ), 1, command->DrawBitmap.size );

	GLsizei stride = sizeof( VertexBitmap );
	int n_verts = data->vbo_n / stride;
	auto uv_offset = (const void*)sizeof( VertexBitmap::pos );

	glBindBuffer( GL_ARRAY_BUFFER, data->vbo );
	glVertexAttribPointer( glGetAttribLocation( data->shader, "pos" ), 3, GL_FLOAT, GL_FALSE, stride, 0 );
	glVertexAttribPointer( glGetAttribLocation( data->shader, "uv" ), 2, GL_FLOAT, GL_FALSE, stride, uv_offset );
//	glVertexAttribPointer( glGetAttribLocation( data.shader, "col" ), 3, GL_FLOAT, GL_FALSE, stride, 0 );

// TODO bind relevant texture here.
// TODO only activate and rebind if neccessary.
//	int texture_i = 0;
//	glActiveTexture( GL_TEXTURE0 + texture_i );
//	glUniform1i( glGetUniformLocation( command->DrawBitmap.shader, "texture" ), texture_i );

//	glBindTexture( GL_TEXTURE_2D, command->DrawBitmap.bitmap );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, n_verts );
};

class RenderFuncs
{
public:
	xgfx::RenderFunc draw_bitmap;
};

RenderFuncs render_funcs_OpenGL
{
	( xgfx::RenderFunc ) draw_bitmap,
};
