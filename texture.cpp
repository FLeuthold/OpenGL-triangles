#include <sb7.h>
#include <vmath.h>
#include <sb7ktx.h>
#include <string>
static void print_shader_log(GLuint shader)
{
	std::string str;
	GLint len;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len != 0)
	{
		str.resize(len);
		glGetShaderInfoLog(shader, len, NULL, &str[0]);
	}

#ifdef _WIN32
	OutputDebugStringA(str.c_str());
#endif
}

static const char * vs_source[] =
{
	"#version 420 core                                                              \n"
	"                                                                               \n"
	"void main(void)                                                                \n"
	"{                                                                              \n"
	"    const vec4 vertices[] = vec4[](vec4( 0.75, -0.75, 0.5, 1.0),               \n"
	"                                   vec4(-0.75, -0.75, 0.5, 1.0),               \n"
	"                                   vec4( 0.75,  0.75, 0.5, 1.0));              \n"
	"                                                                               \n"
	"    gl_Position = vertices[gl_VertexID];                                       \n"
	"}                                                                              \n"
};

static const char * fs_source[] =
{
	"#version 430 core                                                              \n"
	"                                                                               \n"
	"uniform sampler2D s;                                                           \n"
	"                                                                               \n"
	"out vec4 color;                                                                \n"
	"                                                                               \n"
	"void main(void)                                                                \n"
	"{                                                                              \n"
	"    color = texture(s, gl_FragCoord.xy / textureSize(s, 0));                   \n"
	"}                                                                              \n"
};

class simpletexture_app : public sb7::application
{
public:
	void init()
	{
		static const char title[] = "OpenGL SuperBible - Simple Texturing";

		sb7::application::init();

		memcpy(info.title, title, sizeof(title));
	}

	void startup(void)
	{
		// Generate a name for the texture
		glGenTextures(1, &texture);

		// Load texture from file
		sb7::ktx::file::load("media/textures/tree.ktx", texture);
		
		
		// Now bind it to the context using the GL_TEXTURE_2D binding point
		glBindTexture(GL_TEXTURE_2D, texture);
		
		program = glCreateProgram();
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, fs_source, NULL);
		glCompileShader(fs);

		//print_shader_log(fs);

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, vs_source, NULL);
		glCompileShader(vs);

		//print_shader_log(vs);

		glAttachShader(program, vs);
		glAttachShader(program, fs);

		glLinkProgram(program);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	}

	void shutdown(void)
	{
		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vao);
		glDeleteTextures(1, &texture);
	}

	void render(double t)
	{
		static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, green);

		glUseProgram(program);
		//glViewport(0, 0, info.windowWidth, info.windowHeight);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}


private:
	GLuint      texture;
	GLuint      program;
	GLuint      vao;
};

DECLARE_MAIN(simpletexture_app);
