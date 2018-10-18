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
			"void main(void) {                            \n"
			"    gl_Position =  position;                 \n"
			"}                                            \n"
		};
		static const char * fs_1_source[] =
		{
			"#version 450 core                                          \n"	
			"layout (binding = 3, offset = 0) uniform atomic_uint area; \n"
			"                                                           \n"
			"void main(void) {                                          \n"
			"	 atomicCounterIncrement(area);                          \n"
			//"    memoryBarrierAtomicCounter();                          \n"
			"}                                                          \n"
		};
		static const char * fs_2_source[] =
		{
			"#version 450 core                            \n"
			"layout (binding = 3) uniform area_block {    \n"
			"	uint counter_value;                       \n"
			"};                                           \n"
			"out vec4 color;                              \n"
			"layout(location = 4) uniform float max_area; \n"
			"                                             \n"
			"void main(void){                             \n"			
			"	float brightness = clamp(float(counter_value)*10 / max_area, 0.0, 1.0); \n"
			"   color = vec4(brightness, brightness, brightness , 1.0);               \n"
			"}                                                                       \n"
		};

		program_1 = glCreateProgram();

		GLuint fs_1 = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs_1, 1, fs_1_source, NULL);
		glCompileShader(fs_1);
		
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, vs_source, NULL);
		glCompileShader(vs);

		glAttachShader(program_1, vs);		
		glAttachShader(program_1, fs_1);
		glLinkProgram(program_1);
		glDeleteShader(fs_1);
		glDeleteShader(vs);

		/////////////////////////////

		program_2 = glCreateProgram();

		GLuint fs_2 = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs_2, 1, fs_2_source, NULL);
		glCompileShader(fs_2);

		glAttachShader(program_2, fs_2);
		glAttachShader(program_2, vs);
		glLinkProgram(program_2);
		glDeleteShader(fs_2);

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

		//name a buffer
		glCreateBuffers(1, &buffer);
		glNamedBufferStorage(buffer, sizeof(vertices), NULL, GL_MAP_WRITE_BIT);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		//passing vertex data to a buffer
		void * ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(ptr, vertices, sizeof(vertices));
		glUnmapBuffer(GL_ARRAY_BUFFER);

		//Position
		glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
		glVertexArrayAttribBinding(vao, 0, 0);
		glEnableVertexArrayAttrib(vao, 0);

		//Color
		glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
		glVertexArrayAttribBinding(vao, 1, 0);
		glEnableVertexArrayAttrib(vao, 1);
		
		glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(vertex));

		//max_area
		glUseProgram(program_2);
		glUniform1f(4, (float) info.windowHeight * (float) info.windowWidth);			
	}

	virtual void render(double currentTime) {	
		//reset buffer
		//name / create a counter buffer for fs_2		
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buf);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, 16 * sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 3, buf);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buf);
		const GLuint zero = 0;
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 2 * sizeof(GLuint), sizeof(GLuint), &zero);
		
		//draw the first time		
		glUseProgram(program_1);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//pass counted area to buffer
		glBindBufferBase(GL_UNIFORM_BUFFER, 3, buf);

		//draw the second time
		glUseProgram(program_2);		
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	virtual void shutdown(){
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(program_1);
		glDeleteProgram(program_2);
		glDeleteBuffers(1, &buffer);
		glDeleteBuffers(1, &buf);
	}

private:
	GLuint          program_1;
	GLuint			program_2;
	GLuint          vao;
	GLuint          buffer;
	GLuint          buf;

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