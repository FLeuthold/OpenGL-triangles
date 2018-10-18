#include "sb7.h"
#include <math.h>
#include "vmath.h"
class my_application : public sb7::application {

	virtual void startup()
	{
		static const char * vs_source[] =
		{
			"#version 450 core                            \n"
			"layout (location = 0) in vec4 position;      \n"
			"layout (location = 1) in vec4 color;         \n"
			"out vec4 vs_color;							  \n"
			"layout (location = 2) uniform mat4 mvp;      \n"
			"void main(void)                              \n"
			"{                                            \n"
			"    gl_Position =   mvp * position;          \n" // 
			"    vs_color = color;                        \n"
			"}                                            \n"
		};

		static const char * fs_source[] =
		{
			"#version 450 core                            \n"
			"                                             \n"
			"in vec4 vs_color;                            \n"
			"out vec4 color;                              \n"
			"                                             \n"
			"void main(void)                              \n"
			"{                                            \n"
			"    color = vs_color;                        \n"
			"}                                            \n"
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
		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		void * ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(ptr, vertices, sizeof(vertices));
		glUnmapBuffer(GL_ARRAY_BUFFER);//GL_ARRAY_BUFFER

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
	}

	virtual void render(double currentTime) {
		const GLfloat color[] = {(float)sin(currentTime)*0.5f + 0.5f, (float)cos(currentTime)*0.5f + 0.5f, 0.0f, 1.0f};
		glClearBufferfv(GL_COLOR, 0, color);
		
		vmath::mat4 mv_matrix = vmath::translate(0.0f, -0.0f, -3.0f) * vmath::rotate((float) currentTime * 30.0f, 0.0f, 0.5f, 1.0f);
		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 10.0f);
		glUseProgram(program);
		glUniformMatrix4fv(2, 1, GL_FALSE, proj_matrix * mv_matrix);
		
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	virtual void shutdown(){
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(program);
		//glDeleteBuffers(1, &buffer);
	}

private:
	GLuint          program;
	GLuint          vao;
	GLuint          buffer;

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
	/*struct vertex {
		vmath::vec4 pos;
		vmath::vec4 color;
	};*/
};
DECLARE_MAIN(my_application);