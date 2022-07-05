#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "texture.h"

#include "file.h"
#include "shader.h"
#include "model.h"

#define WINDOW_WIDTH		1280
#define WINDOW_HEIGHT		720
#define WINDOW_TITLE		"Hungover"

#define PLAYER_MOVE_SPEED	4.0f
#define PLAYER_LERP_SPEED	8.0f
#define CAM_SENSITIVITY		0.16f

void error_callback(int a, const char *err) {
	printf("ERROR: %s CODE: %d\n", err, a);
}

int main(void) {
	GLFWwindow *window;

	GLubyte draw_mode = 0;
	GLubyte first_press = 0;

	GLdouble time_now;
	GLdouble time_last;
	GLdouble time_delta;
	GLdouble time_elapsed = 0.0;

	GLdouble mouse_x, mouse_y;

	player_t player;

	GLuint shader_program;
	GLuint light_shader_program;
	texture_t textures[2];
	texture_t test_textures[6];
	mesh_t mesh_cube;
	mesh_t mesh_light;

	model_t model_room;

	mat4 matrix_projection;
	mat4 matrix_view;

	vec3 light_color;
	vec3 light_ambient_color;
	vec3 light_diffuse_color;

	vertex_t vertices[] = {
		{{-0.5f, -0.5f, -0.5f},	 {0.0f,  0.0f, -1.0f},	{0.0f, 0.0f}}, // front face
		{{ 0.5f, -0.5f, -0.5f},	 {0.0f,  0.0f, -1.0f},	{1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},	 {0.0f,  0.0f, -1.0f},	{0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f},	 {0.0f,  0.0f, -1.0f},	{1.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f},	 {0.0f,  0.0f,  1.0f},	{1.0f, 0.0f}}, // back face
		{{ 0.5f, -0.5f,  0.5f},	 {0.0f,  0.0f,  1.0f},	{0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},	 {0.0f,  0.0f,  1.0f},	{1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},	 {0.0f,  0.0f,  1.0f},	{0.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f},	{-1.0f,  0.0f,  0.0f},	{0.0f, 0.0f}}, // left face
		{{-0.5f, -0.5f, -0.5f},	{-1.0f,  0.0f,  0.0f},	{1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},	{-1.0f,  0.0f,  0.0f},	{0.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f},	{-1.0f,  0.0f,  0.0f},	{1.0f, 1.0f}},

		{{ 0.5f, -0.5f,  0.5f},	 {1.0f,  0.0f,  0.0f},	{1.0f, 0.0f}}, // right face
		{{ 0.5f, -0.5f, -0.5f},	 {1.0f,  0.0f,  0.0f},	{0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},	 {1.0f,  0.0f,  0.0f},	{1.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f},	 {1.0f,  0.0f,  0.0f},	{0.0f, 1.0f}},

		{{-0.5f,  0.5f, -0.5f},	 {0.0f,  1.0f,  0.0f},	{0.0f, 0.0f}}, // top face
		{{ 0.5f,  0.5f, -0.5f},	 {0.0f,  1.0f,  0.0f},	{1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},	 {0.0f,  1.0f,  0.0f},	{0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},	 {0.0f,  1.0f,  0.0f},	{1.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f},	 {0.0f, -1.0f,  0.0f},	{0.0f, 0.0f}}, // bottom face
		{{ 0.5f, -0.5f, -0.5f},	 {0.0f, -1.0f,  0.0f},	{1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},	 {0.0f, -1.0f,  0.0f},	{0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f},	 {0.0f, -1.0f,  0.0f},	{1.0f, 1.0f}},
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
		{-4.0f,  2.5f,  -3.0f},
		{-4.0f,  2.5f,   3.0f},
		{ 4.0f,  2.5f,   3.0f},
		{ 4.0f,  2.5f,   0.25f},
	};

	glm_vec3_copy(GLM_VEC3_ZERO, player.headbob);
	glm_vec3_copy(GLM_VEC3_ONE, light_color);
	glm_vec3_scale(light_color, 0.5f, light_diffuse_color);
	glm_vec3_scale(light_diffuse_color, 0.2f, light_ambient_color);

	glm_perspective(glm_rad(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f, matrix_projection);
	glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_view);

	/* camera initialization */
	glm_vec3_copy(GLM_YUP, player.up);
	glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, player.pos);
	glm_vec3_copy(player.pos, player.pos_end);
	glm_vec2_copy((vec2){0.0f, 0.0f}, player.rot);

	if(!glfwInit()) {
		printf("ERROR: GLFW fucked up.\n");
		return 1;
	}

	glfwSetErrorCallback(error_callback);

	/* creating a window */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

	/* loading shaders */
	shader_program = shader_create("res/shaders/vert.glsl", "res/shaders/frag.glsl");
	glUseProgram(shader_program);
	glUniform3f(glGetUniformLocation(shader_program, "material.specular_color"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(shader_program, "material.shininess"), 32.0f);

	glUniform3f(glGetUniformLocation(shader_program, "light_dir.dir"), 0.0f, 0.0f, -1.0f);
	glUniform3fv(glGetUniformLocation(shader_program, "light_dir.ambient_color"), 1, (const GLfloat *)light_ambient_color);
	glUniform3fv(glGetUniformLocation(shader_program, "light_dir.diffuse_color"), 1, (const GLfloat *)light_diffuse_color);
	glUniform3f(glGetUniformLocation(shader_program, "light_dir.specular_color"), 1.0f, 1.0f, 1.0f);

	for(GLuint i = 0; i < 4; i++) {
		char buffer[128] = { 0 };
		sprintf(buffer, "light_points[%d].pos", i);
		glUniform3fv(glGetUniformLocation(shader_program, buffer), 1, (const GLfloat *)light_point_positions[i]);
		sprintf(buffer, "light_points[%d].ambient_color", i);
		glUniform3fv(glGetUniformLocation(shader_program, buffer), 1, (const GLfloat *)light_ambient_color);
		sprintf(buffer, "light_points[%d].diffuse_color", i);
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

	light_shader_program = shader_create("res/shaders/light-vert.glsl", "res/shaders/light-frag.glsl");
	glUseProgram(light_shader_program);

	/* loading textures */
	textures[0] = texture_create("res/textures/box-diffuse.png", TT_DIFFUSE);
	textures[1] = texture_create("res/textures/box-specular.png", TT_SPECULAR);

	test_textures[0] = texture_create("res/models/room/floor-diffuse.png", TT_DIFFUSE);
	test_textures[1] = texture_create("res/models/room/black.png", TT_SPECULAR);
	test_textures[2] = texture_create("res/models/room/wall-diffuse.png", TT_DIFFUSE);
	test_textures[3] = test_textures[1];
	test_textures[4] = texture_create("res/models/room/ceiling-diffuse.png", TT_DIFFUSE);
	test_textures[5] = texture_create("res/models/room/ceiling-specular.png", TT_SPECULAR);

	/* loading meshes */
	model_room = model_create("res/models/room/room.glb", test_textures, 6);
	mesh_cube = mesh_create(vertices, indices, textures, sizeof(vertices) / sizeof(vertex_t), sizeof(indices) / sizeof(GLuint), 2);
	mesh_light = mesh_create(vertices, indices, NULL, sizeof(vertices) / sizeof(vertex_t), sizeof(indices) / sizeof(GLuint), 0);

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, (double)WINDOW_WIDTH / 2, (double)WINDOW_HEIGHT / 2);

	time_now = glfwGetTime();
	time_last = time_now;

	/* main loop */
	while(!glfwWindowShouldClose(window)) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);

		time_now = glfwGetTime();
		time_delta = time_now - time_last;
		time_last = time_now;
		time_elapsed += time_delta;

		/* camera look */
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		mouse_x -= (double)WINDOW_WIDTH / 2;
		mouse_y -= (double)WINDOW_HEIGHT / 2;

		player.rot[0] += mouse_x * CAM_SENSITIVITY;
		player.rot[1] -= mouse_y * CAM_SENSITIVITY;
		if(player.rot[1] > 89.0f)
			player.rot[1] = 89.0f;

		if(player.rot[1] < -89.0f)
			player.rot[1] = -89.0f;

		if(player.rot[0] < -180.0f)
			player.rot[0] = 180.0f;

		if(player.rot[0] > 180.0f)
			player.rot[0] = -180.0f;

		printf("%f\n", player.rot[0]);

		player.dir[0] = cos(glm_rad(player.rot[0])) * cos(glm_rad(player.rot[1]));
		player.dir[1] = sin(glm_rad(player.rot[1]));
		player.dir[2] = sin(glm_rad(player.rot[0])) * cos(glm_rad(player.rot[1]));
		glm_normalize(player.dir);

		glm_cross(player.dir, player.up, player.right);
		glm_normalize(player.right);

		glfwSetCursorPos(window, (double)WINDOW_WIDTH / 2, (double)WINDOW_HEIGHT / 2);

		/* toggling wireframe */
		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !first_press) {
			draw_mode = !draw_mode;
			first_press = 1;
		}

		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
			first_press = 0;

		{ /* handing player movement */
			const float player_speed = (float)time_delta * PLAYER_MOVE_SPEED;
			vec3 player_move;

			glm_vec3_copy(GLM_VEC3_ZERO, player_move);
			if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				glm_vec3_add(player_move, player.right, player_move);

			if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				glm_vec3_sub(player_move, player.right, player_move);

			if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				glm_vec3_add(player_move, player.dir, player_move);

			if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				glm_vec3_sub(player_move, player.dir, player_move);

			glm_vec3_normalize(player_move);
			glm_vec3_scale(player_move, player_speed, player_move);
			glm_vec3_add(player.pos_end, player_move, player.pos_end);
		}

		/* TODO: Implement jumping */
		if(player.pos_end[1] < 1.0f || player.pos_end[1] > 1.0f)
			player.pos_end[1] = 1.0f;

		{ /* lerping players position */
			vec3 old_pos;
			vec3 move_vec;
			glm_vec3_copy(player.pos, old_pos);
			glm_vec3_lerp(player.pos, player.pos_end, (float)time_delta * PLAYER_LERP_SPEED, player.pos);
			glm_vec3_sub(player.pos, old_pos, move_vec);
			player.move_delta = glm_vec3_norm(move_vec);
		}

		glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_view);

		player.headbob[0] = cosf(time_elapsed * GLM_PIf * 2) * player.move_delta;
		player.headbob[1] = sinf(time_elapsed * GLM_PIf * 4) * player.move_delta * 0.66f;
		player.headbob[2] = cosf(time_elapsed * GLM_PIf * 2) * player.move_delta;

		player.headbob[0] *= -sinf(glm_rad(player.rot[0]));
		player.headbob[2] *= cosf(glm_rad(player.rot[0]));

		glm_vec3_add(player.pos, player.dir, player.tar);
		glm_vec3_add(player.pos, player.headbob, player.pos_bobbed);
		glm_vec3_add(player.tar, player.headbob, player.tar_bobbed);
		glm_lookat(player.pos_bobbed, player.tar_bobbed, player.up, matrix_view);

		glm_vec3_scale(light_color, 0.5f, light_diffuse_color);
		glm_vec3_scale(light_diffuse_color, 0.2f, light_ambient_color);

		/* drawing */
		if(draw_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, (const GLfloat *)matrix_view);
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, (const GLfloat *)matrix_projection);
		glUniform3fv(glGetUniformLocation(shader_program, "view_pos"), 1, (const GLfloat *)player.pos);
		for(GLuint i = 0; i < 10; i++) {
			mat4 model_mat;
			float angle = 20 * i + (time_elapsed * GLM_PI * 12);
			glm_mat4_copy(GLM_MAT4_IDENTITY, model_mat);
			glm_translate(model_mat, cube_positions[i]);
			glm_rotate(model_mat, glm_rad(angle), (vec3){1.0f, 0.3f, 0.5f});
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, (const GLfloat *)model_mat);
			// mesh_draw(mesh_cube, shader_program);
		}

		{
			mat4 model_room_mat;
			glm_mat4_copy(GLM_MAT4_IDENTITY, model_room_mat);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[0].id);
			glUniform1i(glGetUniformLocation(shader_program, "material.tex_diffuse0"), 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures[1].id);
			glUniform1i(glGetUniformLocation(shader_program, "material.tex_specular0"), 1);

			glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, (const GLfloat *)model_room_mat);
			model_draw(model_room, shader_program);
		}

		glUseProgram(light_shader_program);
		glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "view"), 1, GL_FALSE, (const GLfloat *)matrix_view);
		glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "projection"), 1, GL_FALSE, (const GLfloat *)matrix_projection);
		glUniform3fv(glGetUniformLocation(light_shader_program, "light_color"), 1, (const GLfloat *)light_color);
		for(GLuint i = 0; i < 4; i++) {
			mat4 light_mat;
			glm_mat4_copy(GLM_MAT4_IDENTITY, light_mat);
			glm_translate(light_mat, light_point_positions[i]);
			glm_scale(light_mat, (vec3){0.2f, 0.2f, 0.2f});
			glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "model"), 1, GL_FALSE, (const GLfloat *)light_mat);
			mesh_draw(mesh_light, light_shader_program);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	model_destroy(&model_room);
	mesh_destroy(&mesh_light);
	mesh_destroy(&mesh_cube);
	texture_destroy(&textures[1]);
	texture_destroy(&textures[0]);
	glDeleteShader(light_shader_program);
	glDeleteShader(shader_program);

	glfwTerminate();

	return 0;
}
