#pragma once

#include <render_func.h>
#include <render_command.h>

struct VertexBitmap
{
	float pos[ 3 ];
	float uv[ 2 ];
};

class RenderFuncs
{
public:
	Raphael::RenderFunc draw_bitmap;
};

RenderFuncs render_funcs_OpenGL
{
	// Draw bitmap.
	[]( const Raphael::RenderCommand *command )
	{
		glUseProgram( command->DrawBitmap.shader );

		glUniformMatrix4fv( glGetUniformLocation( command->DrawBitmap.shader, "mvp" ), 1, GL_FALSE, command->DrawBitmap.mvp );
//		glUniform2fv( glGetUniformLocation( command->DrawBitmap.shader, "size" ), 1, command->DrawBitmap.size );
//
		GLsizei stride = sizeof( VertexBitmap );
		int n_verts = command->DrawBitmap.vbo_n / stride;
		auto uv_offset = (const void*)sizeof( VertexBitmap::pos );

		glBindBuffer( GL_ARRAY_BUFFER, command->DrawBitmap.vbo );
		glVertexAttribPointer( glGetAttribLocation( command->DrawBitmap.shader, "pos" ), 3, GL_FLOAT, GL_FALSE, stride, 0 );
		glVertexAttribPointer( glGetAttribLocation( command->DrawBitmap.shader, "uv" ), 2, GL_FLOAT, GL_FALSE, stride, uv_offset );
		//glVertexAttribPointer( glGetAttribLocation( data.shader, "col" ), 3, GL_FLOAT, GL_FALSE, stride, 0 );

		// TODO bind relevant texture here.
		// TODO only activate and rebind if neccessary.
//		int texture_i = 0;
//		glActiveTexture( GL_TEXTURE0 + texture_i );
//		glUniform1i( glGetUniformLocation( command->DrawBitmap.shader, "texture" ), texture_i );

//		glBindTexture( GL_TEXTURE_2D, command->DrawBitmap.bitmap );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, n_verts );
	},
};
