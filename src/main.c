#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"

#define WINDOW_WIDTH	1280
#define WINDOW_HEIGHT	720
#define WINDOW_TITLE	"Attempt Numero Dos"

int main(void) {
	GLFWwindow *window;

	GLuint vao;
	GLuint vbo;

	GLchar *vertex_shader_source;
	GLchar *fragment_shader_source;

	GLuint shader_program;

	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	if(!glfwInit()) {
		printf("ERROR: GLFW fucked up.\n");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* creating a window */
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if(!window) {
		printf("ERROR: Window fucked up.\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	/* initializing the GLAD plugin loader */
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("ERROR: GLAD fucked up.\n");
		glfwTerminate();
		return 1;
	}

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	/* getting vao set up */
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* getting vbo set up */
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	{ /* loading and compiling vertex shader */
		GLuint vertex_shader;
		GLuint fragment_shader;

		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		vertex_shader_source = file_load_contents("res/shaders/vert.glsl");
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
				return 1;
			}
		}

		/* loading and compiling fragment shader */
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		fragment_shader_source = file_load_contents("res/shaders/frag.glsl");
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
				return 1;
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
				return 1;
			}
		}

		glUseProgram(shader_program);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
	}

	/* main loop */
	while(!glfwWindowShouldClose(window)) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	free(vertex_shader_source);
	glfwTerminate();

	return 0;
}
