#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include "file.h"

GLuint shader_create(const char *vert_path, const char *frag_path) {
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLchar *vertex_shader_source;
	GLchar *fragment_shader_source;

	GLuint shader_program;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	vertex_shader_source = file_load_contents(vert_path);
	if(!vertex_shader_source) {
		printf("ERROR: Couldn't find Vertex Shader code.\n");
		return 0;
	}

	glShaderSource(vertex_shader, 1, (const GLchar * const *)&vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	{
		GLint success;
		GLchar info_log[512];
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
			printf("ERROR: Vertex shader fucked up: %s\n", info_log);
			glfwTerminate();
			return 0;
		}
	}

	/* loading and compiling fragment shader */
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	fragment_shader_source = file_load_contents(frag_path);
	if(!fragment_shader_source) {
		printf("ERROR: Couldn't find Fragment Shader code.\n");
		return 0;
	}

	glShaderSource(fragment_shader, 1, (const GLchar * const *)&fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	{
		GLint success;
		GLchar info_log[512];
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
			printf("ERROR: Fragment shader fucked up: %s\n", info_log);
			glfwTerminate();
			return 0;
		}
	}

	/* creating shader program and binding vert and frag */
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	{
		GLint success;
		GLchar info_log[512];
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
		if(!success) {
			glGetProgramInfoLog(shader_program, 512, NULL, info_log);
			printf("ERROR: Shader program fucked up: %s\n", info_log);
			glfwTerminate();
			return 0;
		}
	}

	glUseProgram(shader_program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	free(vertex_shader_source);
	free(fragment_shader_source);

	return shader_program;
}
