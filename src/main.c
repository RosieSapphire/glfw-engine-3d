#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "file.h"
#include "shader.h"
#include "camera.h"

#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	800
#define WINDOW_TITLE	"Attempt Numero Dos"

#define CAM_MOVE_SPEED	12.0f
#define CAM_SENSITIVITY	0.16f

int main(void) {
	GLFWwindow *window;

	GLubyte draw_mode = 0;
	GLubyte first_press = 0;

	GLdouble time_now;
	GLdouble time_last;
	GLdouble time_delta;
	GLdouble time_elapsed = 0.0;

	GLdouble mouse_x, mouse_y;

	camera_t cam;

	GLuint vao, vbo, ebo;
	GLuint vao_light;

	GLuint shader_program;
	GLuint light_shader_program;

	/*
	GLuint texture;
	GLint texture_width, texture_height, texture_channels;
	GLubyte *texture_data;
	*/

	mat4 matrix_model;
	mat4 matrix_projection;
	mat4 matrix_view;

	mat4 light_model;
	vec3 light_color;
	vec3 light_pos;

	GLfloat vertices[] = {
		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f, // front face
		 1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
		 1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,

		-1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f, // back face
		 1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,

		-1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f, // left face
		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,

		 1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f, // right face
		 1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		 1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,

		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f, // top face
		 1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,

		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f, // bottom face
		 1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 1.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,	1.0f, 1.0f, 1.0f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
	};

	GLuint indices[] = {
		0, 1, 2,
		2, 1, 3,

		4, 5, 6,
		6, 5, 7,

		8, 9, 10,
		10, 9, 11,

		12, 13, 14,
		14, 13, 15,

		16, 17, 18,
		18, 17, 19,

		20, 21, 22,
		22, 21, 23
	};

	glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, light_color);
	glm_vec3_copy((vec3){0.0f, 3.0f, 0.0f}, light_pos);

	glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_model);
	glm_perspective(glm_rad(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 1000.0f, matrix_projection);
	glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_view);

	/* camera initialization */
	glm_vec3_copy(GLM_YUP, cam.up);
	glm_vec3_copy((vec3){0.0f, 0.0f, -4.0f}, cam.pos);
	glm_vec3_copy(cam.pos, cam.pos_end);
	glm_vec2_copy((vec2){90.0f, 0.0f}, cam.rot);

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid *)(9 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	shader_program = shader_create("res/shaders/vert.glsl", "res/shaders/frag.glsl");
	glUseProgram(shader_program);
	glUniform1i(glGetUniformLocation(shader_program, "tex01"), 0);
	glUniform3fv(glGetUniformLocation(shader_program, "light_color"), 1, (const GLfloat *)light_color);

	/* setting up texture-related shit */
	/*
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
	*/

	/* creating the light buffers */
	glGenVertexArrays(1, &vao_light);
	glBindVertexArray(vao_light);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	light_shader_program = shader_create("res/shaders/light-vert.glsl", "res/shaders/light-frag.glsl");
	glUseProgram(light_shader_program);
	glUniform3fv(glGetUniformLocation(light_shader_program, "light_color"), 1, (const GLfloat *)light_color);

	time_now = glfwGetTime();
	time_last = time_now;

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/* main loop */
	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	while(!glfwWindowShouldClose(window)) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);

		time_now = glfwGetTime();
		time_delta = time_now - time_last;
		time_last = time_now;
		time_elapsed += time_delta;

		/* camera look */
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		mouse_x -= WINDOW_WIDTH / 2;
		mouse_y -= WINDOW_HEIGHT / 2;

		cam.rot[0] += mouse_x * CAM_SENSITIVITY;
		cam.rot[1] -= mouse_y * CAM_SENSITIVITY;

		cam.dir[0] = cos(glm_rad(cam.rot[0])) * cos(glm_rad(cam.rot[1]));
		cam.dir[1] = sin(glm_rad(cam.rot[1]));
		cam.dir[2] = sin(glm_rad(cam.rot[0])) * cos(glm_rad(cam.rot[1]));
		glm_normalize(cam.dir);

		glm_cross(cam.dir, cam.up, cam.right);
		glm_normalize(cam.right);

		glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

		/* toggling wireframe */
		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !first_press) {
			draw_mode = !draw_mode;
			first_press = 1;
		}

		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
			first_press = 0;

		{ /* handing camera movement */
			const float cam_speed = (float)time_delta * CAM_MOVE_SPEED;
			vec3 cam_forward_move;
			vec3 cam_right_move;
			glm_vec3_scale(cam.dir, cam_speed, cam_forward_move);
			glm_vec3_scale(cam.right, cam_speed, cam_right_move);

			if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				glm_vec3_add(cam.pos_end, cam_right_move, cam.pos_end);

			if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				glm_vec3_sub(cam.pos_end, cam_right_move, cam.pos_end);

			if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				glm_vec3_add(cam.pos_end, cam_forward_move, cam.pos_end);

			if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				glm_vec3_sub(cam.pos_end, cam_forward_move, cam.pos_end);
		}

		glm_vec3_lerp(cam.pos, cam.pos_end, (float)time_delta * CAM_MOVE_SPEED, cam.pos);

		glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_model);
		glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_view);

		glm_vec3_add(cam.pos, cam.dir, cam.tar);
		glm_lookat(cam.pos, cam.tar, cam.up, matrix_view);

		light_pos[0] = (float)cos(time_elapsed * GLM_PI) * 3.0f;
		light_pos[2] = (float)sin(time_elapsed * GLM_PI) * 3.0f;
		glm_mat4_copy(GLM_MAT4_IDENTITY, light_model);
		glm_translate(light_model, light_pos);
		glm_scale(light_model, (vec3){0.2f, 0.2f, 0.2f});

		/* drawing */
		if(draw_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, (const GLfloat *)matrix_model);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, (const GLfloat *)matrix_view);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, (const GLfloat *)matrix_projection);
		glUniform3fv(glGetUniformLocation(shader_program, "view_pos"), 1, (const GLfloat *)cam.pos);
		glUniform3fv(glGetUniformLocation(shader_program, "light_pos"), 1, (const GLfloat *)light_pos);

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		*/

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / 3, GL_UNSIGNED_INT, 0);

		glUseProgram(light_shader_program);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, (const GLfloat *)light_model);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, (const GLfloat *)matrix_view);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, (const GLfloat *)matrix_projection);

		glBindVertexArray(vao_light);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / 3, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteShader(shader_program);
	glDeleteShader(light_shader_program);
	glfwTerminate();

	return 0;
}
