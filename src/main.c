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

	GLuint tex_diffuse;
	GLint tex_diffuse_width, tex_diffuse_height, tex_diffuse_channels;
	GLubyte *tex_diffuse_data;

	GLuint tex_specular;
	GLint tex_specular_width, tex_specular_height, tex_specular_channels;
	GLubyte *tex_specular_data;

	mat4 matrix_model;
	mat4 matrix_projection;
	mat4 matrix_view;

	vec3 light_color;
	vec3 light_ambient_color;
	vec3 light_diffuse_color;

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f, // front face
		 0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f, // back face
		 0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f, // left face
		-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,

		 0.5f, -0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f, // right face
		 0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f, // top face
		 0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f, // bottom face
		 0.5f, -0.5f, -0.5f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
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

	vec3 cube_positions[] = {
		{ 0.0f,  0.0f,   0.0f},
		{ 2.0f,  5.0f, -15.0f},
		{-1.5f, -2.2f,  -2.5f},
		{-3.8f, -2.0f, -12.3f},
		{ 2.4f, -0.4f,  -3.5f},
		{-1.7f,  3.0f,  -7.5f},
		{ 1.3f, -2.0f,  -2.5f},
		{ 1.5f,  2.0f,  -2.5f},
		{ 1.5f,  0.2f,  -1.5f},
		{-1.3f,  1.0f,  -1.5f}
	};

	vec3 light_point_positions[] = {
		{ 0.7f,  0.2f,   2.0f},
		{ 2.3f, -3.3f,  -4.0f},
		{-4.0f,  2.0f, -12.0f},
		{ 0.0f,  0.0f,  -3.0f},
	};

	glm_vec3_copy(GLM_VEC3_ONE, light_color);
	glm_vec3_scale(light_color, 0.5f, light_diffuse_color);
	glm_vec3_scale(light_diffuse_color, 0.2f, light_ambient_color);

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	shader_program = shader_create("res/shaders/vert.glsl", "res/shaders/frag.glsl");
	glUseProgram(shader_program);
	glUniform1i(glGetUniformLocation(shader_program, "material.diffuse_tex"), 0);
	glUniform1i(glGetUniformLocation(shader_program, "material.specular_tex"), 1);
	glUniform3f(glGetUniformLocation(shader_program, "material.specular_color"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(shader_program, "material.shininess"), 32.0f);

	glUniform3f(glGetUniformLocation(shader_program, "light_dir.dir"), 0.0f, 0.0f, -1.0f);
	glUniform3fv(glGetUniformLocation(shader_program, "light_dir.ambient_color"), 1, (const GLfloat *)light_ambient_color);
	glUniform3fv(glGetUniformLocation(shader_program, "light_dir.diffuse_color"), 1, (const GLfloat *)light_diffuse_color);
	glUniform3f(glGetUniformLocation(shader_program, "light_dir.specular_color"), 1.0f, 1.0f, 1.0f);

	for(GLuint i = 0; i < 4; i++) {
		char buffer[128];
		sprintf(buffer, "light_points[%d].pos", i);
		glUniform3fv(glGetUniformLocation(shader_program, buffer), 1, (const GLfloat *)light_point_positions[i]);
		sprintf(buffer, "light_points[%d].ambient_color", i);
		glUniform3fv(glGetUniformLocation(shader_program, buffer), 1, (const GLfloat *)light_ambient_color);
		sprintf(buffer, "light_point[%d].diffuse_color", i);
		glUniform3fv(glGetUniformLocation(shader_program, buffer), 1, (const GLfloat *)light_diffuse_color);
		sprintf(buffer, "light_points[%d].specular_color", i);
		glUniform3f(glGetUniformLocation(shader_program, buffer), 1.0f, 1.0f, 1.0f);
		sprintf(buffer, "light_points[%d].constant", i);
		glUniform1f(glGetUniformLocation(shader_program, buffer), 1.0f);
		sprintf(buffer, "light_points[%d].linear", i);
		glUniform1f(glGetUniformLocation(shader_program, buffer), 0.09f);
		sprintf(buffer, "light_points[%d].quadratic", i);
		glUniform1f(glGetUniformLocation(shader_program, buffer), 0.032f);
	}

	/* loading diffuse texture */
	glGenTextures(1, &tex_diffuse);
	glBindTexture(GL_TEXTURE_2D, tex_diffuse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(1);
	tex_diffuse_data = stbi_load("res/textures/box-diffuse.png", &tex_diffuse_width, &tex_diffuse_height, &tex_diffuse_channels, 0);
	if(!tex_diffuse_data) {
		printf("ERROR: Diffuse texture fucked up.\n");
		glfwTerminate();
		return 1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_diffuse_width, tex_diffuse_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_diffuse_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(tex_diffuse_data);

	/* loading specular texture */
	glGenTextures(1, &tex_specular);
	glBindTexture(GL_TEXTURE_2D, tex_specular);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(1);
	tex_specular_data = stbi_load("res/textures/box-specular.png", &tex_specular_width, &tex_specular_height, &tex_specular_channels, 0);
	if(!tex_specular_data) {
		printf("ERROR: Specular texture fucked up.\n");
		glfwTerminate();
		return 1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_specular_width, tex_specular_height, 0, GL_RED, GL_UNSIGNED_BYTE, tex_specular_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(tex_specular_data);

	/* creating the light buffers */
	glGenVertexArrays(1, &vao_light);
	glBindVertexArray(vao_light);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	light_shader_program = shader_create("res/shaders/light-vert.glsl", "res/shaders/light-frag.glsl");
	glUseProgram(light_shader_program);

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

		glm_vec3_scale(light_color, 0.5f, light_diffuse_color);
		glm_vec3_scale(light_diffuse_color, 0.2f, light_ambient_color);

		/* drawing */
		if(draw_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, (const GLfloat *)matrix_model);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, (const GLfloat *)matrix_view);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, (const GLfloat *)matrix_projection);
		glUniform3fv(glGetUniformLocation(shader_program, "view_pos"), 1, (const GLfloat *)cam.pos);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_diffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_specular);

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		for(GLuint i = 0; i < 10; i++) {
			mat4 model_mat;
			float angle = 20 * i + (time_elapsed * GLM_PI * 12);
			glm_mat4_copy(GLM_MAT4_IDENTITY, model_mat);
			glm_translate(model_mat, cube_positions[i]);
			glm_rotate(model_mat, glm_rad(angle), (vec3){1.0f, 0.3f, 0.5f});
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, (const GLfloat *)model_mat);
			glDrawElements(GL_TRIANGLES, sizeof(indices) / 3, GL_UNSIGNED_INT, 0);
		}

		glUseProgram(light_shader_program);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, (const GLfloat *)matrix_view);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, (const GLfloat *)matrix_projection);
		glUniform3fv(glGetUniformLocation(light_shader_program, "light_color"), 1, (const GLfloat *)light_color);

		glBindVertexArray(vao_light);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		for(GLuint i = 0; i < 4; i++) {
			mat4 light_mat;
			glm_mat4_copy(GLM_MAT4_IDENTITY, light_mat);
			glm_translate(light_mat, light_point_positions[i]);
			glm_scale(light_mat, (vec3){0.2f, 0.2f, 0.2f});
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, (const GLfloat *)light_mat);
			glDrawElements(GL_TRIANGLES, sizeof(indices) / 3, GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteShader(shader_program);
	glDeleteShader(light_shader_program);
	glfwTerminate();

	return 0;
}
