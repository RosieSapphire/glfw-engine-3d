#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "file.h"
#include "shader.h"

#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	800
#define WINDOW_TITLE	"Attempt Numero Dos"

int main(void) {
	GLFWwindow *window;

	GLubyte draw_mode = 0;
	GLubyte first_press = 0;

	GLdouble time_elapsed = 0.0f;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	GLuint shader_program;

	GLuint texture;
	GLint texture_width, texture_height, texture_channels;
	GLubyte *texture_data;

	GLuint texture_1;
	GLint texture_1_width, texture_1_height, texture_1_channels;
	GLubyte *texture_1_data;

	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 1.0f,	1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	};

	GLuint indices[] = {
		0, 1, 2,
		1, 2, 3
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

	/* getting the buffers set up */
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	shader_program = shader_create("res/shaders/vert.glsl", "res/shaders/frag.glsl");

	/* setting up texture-related shit */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(1);
	texture_data = stbi_load("res/textures/test.png", &texture_width, &texture_height, &texture_channels, 0);
	if(!texture_data) {
		printf("ERROR: Texture fucked up.\n");
		glfwTerminate();
		return 1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texture_data);

	glGenTextures(1, &texture_1);
	glBindTexture(GL_TEXTURE_2D, texture_1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texture_1_data = stbi_load("res/textures/test1.png", &texture_1_width, &texture_1_height, &texture_1_channels, 0);
	if(!texture_1_data) {
		printf("ERROR: Texture fucked up.\n");
		glfwTerminate();
		return 1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_1_width, texture_1_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_1_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(texture_1_data);

	/* main loop */
	while(!glfwWindowShouldClose(window)) {
		time_elapsed = glfwGetTime();

		/* input */
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);

		/* toggling wireframe */
		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !first_press) {
			draw_mode = !draw_mode;
			first_press = 1;
		}

		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
			first_press = 0;

		/* drawing */
		if(draw_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		glUniform4f(glGetUniformLocation(shader_program, "u_color"), 0.0f, (GLfloat)((sin(time_elapsed * 3.14159) + 1) / 2), 0.0f, 1.0f);
		glUniform1i(glGetUniformLocation(shader_program, "tex01"), 0);
		glUniform1i(glGetUniformLocation(shader_program, "tex02"), 1);
		glUniform1f(glGetUniformLocation(shader_program, "lerp"), (float)((sin(time_elapsed * 3.14159) + 1) / 2));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_1);

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteShader(shader_program);
	glfwTerminate();

	return 0;
}
