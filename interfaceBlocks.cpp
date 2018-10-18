#include "sb7.h"
#include <math.h>
#include "vmath.h"
#include <iostream>
class my_application : public sb7::application {

	virtual void startup()
	{
		static const char * vs_source[] =
		{
			"#version 450 core                        \n"
			"struct vertex {                          \n"
			"   vec4 position;                        \n"
			"   vec4 color;                           \n"
			"};                                       \n"
			"layout(binding = 0, std430) buffer my_vertices { \n"
			"	vertex vertices[];                    \n"
			"};                                       \n"
			"out VS_OUT {                             \n"
			"   vec4 color;						      \n"
			"} vs_out;      					      \n"
			"layout(binding = 2, std140) uniform TransformBlock {       \n"
			"	layout (offset = 0) mat4 translation;                   \n"
			"	layout (offset = 80) mat4 rotation;                     \n"
			"	layout (offset = 160) mat4 projection_matrix;           \n"
			"};                                                         \n"
			"void main(void) {                                          \n"
			"   mat4 mvp = projection_matrix * translation * rotation ; \n"
			"   gl_Position = mvp * vertices[gl_VertexID].position;     \n"  
			"   vs_out.color = vertices[gl_VertexID].color;             \n"
			"}                                                          \n"
		};

		static const char * fs_source[] =
		{
			"#version 450 core       \n"
			"                        \n"
			"in VS_OUT{              \n"
			"   vec4 color;          \n"
			"} fs_in;                \n"
			"out vec4 color;         \n"
			"                        \n"
			"void main(void){        \n"
			"   color = fs_in.color; \n"
			"}                       \n"
		};

		program = glCreateProgram();
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, fs_source, NULL);
		glCompileShader(fs);

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, vs_source, NULL);
		glCompileShader(vs);

		glAttachShader(program, vs);
		glAttachShader(program, fs);

		glLinkProgram(program);

		//////////////////////////
		static const vertex vertices[] = {
			{ 0.25, -0.25, 0.5, 1.0 ,
			1.0,   0.0,  0.0, 1.0 },

			{ -0.25, -0.25, 0.5, 1.0,
			0.0,   1.0,  0.0, 1.0 },

			{ 0.25,  0.25, 0.5, 1.0,
			0.0,   0.0,  1.0, 1.0 }
		};
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);


		glCreateBuffers(1, &buffer);
		glNamedBufferStorage(buffer, sizeof(vertices), NULL, GL_MAP_WRITE_BIT);//GL_DYNAMIC_STORAGE_BIT
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);

		void * ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		memcpy(ptr, vertices, sizeof(vertices));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);//GL_ARRAY_BUFFER

		//Position
		glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
		glVertexArrayAttribBinding(vao, 0, 0);
		glEnableVertexArrayAttrib(vao, 0);

		//Color
		glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
		glVertexArrayAttribBinding(vao, 1, 0);
		glEnableVertexArrayAttrib(vao, 1);
		
		glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(vertex));

		/////////////////////////////////

		vmath::mat4 transl_matrix = vmath::translate(0.0f, 0.0f, -3.0f);
		vmath::mat4 rot_matrix = vmath::rotate( 30.0f, 0.0f, 0.5f, 1.0f);
		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 10.0f);

		//name a buffer
		glCreateBuffers(1, &buffer1);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, buffer1);
		glNamedBufferStorage(buffer1, 4096, NULL, GL_MAP_WRITE_BIT);

		//allocate memory (thightly packed, that's why it's unsigned char)
		unsigned char * ptr1 = (unsigned char *) glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

		//fill the buffer
		for (int i = 0; i < 4; ++i) {
			GLuint offset = 0 + 4 * sizeof(GLfloat) * i;
			for (int j = 0; j < 4; ++j) {
				*((float *)(ptr1 + offset)) = transl_matrix[i][j];
				offset += sizeof(GLfloat);
			}
		}

		for (int i = 0; i < 4; ++i) {
			GLuint offset = 80 + 4 * sizeof(GLfloat) * i;
			for (int j = 0; j < 4; ++j) {
				*((float *)(ptr1 + offset)) = rot_matrix[i][j];
				offset += sizeof(GLfloat);
			}
		}

		for (int i = 0; i < 4; ++i) {
			GLuint offset = 160 + 4 * sizeof(GLfloat) * i;
			for (int j = 0; j < 4; ++j) {
				*((float *)(ptr1 + offset)) = proj_matrix[i][j];
				offset += sizeof(GLfloat);
			}
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}
	
	virtual void render(double currentTime) {
		const GLfloat color[] = {(float)sin(currentTime)*0.5f + 0.5f, (float)cos(currentTime)*0.5f + 0.5f, 0.0f, 1.0f};
		glClearBufferfv(GL_COLOR, 0, color);
		glUseProgram(program);				
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	virtual void shutdown(){		
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(program);
		glDeleteBuffers(1, &buffer);
		glDeleteBuffers(1, &buffer1);
	}

private:
	GLuint          program;
	GLuint          vao;
	GLuint          buffer;
	GLuint          buffer1;
	

	struct vertex {
		// Position
		float x;
		float y;
		float z;
		float w;

		// Color
		float r;
		float g;
		float b;
		float a;
	};
};
DECLARE_MAIN(my_application);