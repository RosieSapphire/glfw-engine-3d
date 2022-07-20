#include <cglm/mat4.h>
#include <cglm/quat.h>
#include <cglm/vec3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "texture.h"

#include "file.h"
#include "shader.h"
#include "model.h"

#define WINDOW_TITLE		"Hungover"

#define PLAYER_MOVE_SPEED	2.3f
#define PLAYER_LERP_SPEED	8.0f
#define HEADBOB_LERP_SPEED	4.2f
#define HEADBOB_INTENSITY	0.08f
#define CAM_SENSITIVITY		0.16f

/*void error_callback(int a, const char *err) {
	printf("ERROR: %s CODE: %d\n", err, a);
}*/

int main(void) {
	const GLuint sample_count = 4;

	GLFWwindow *window;
	GLfloat window_width, window_height;
	GLfloat aspect_ratio;

	GLuint fbo;
	GLuint fbo_multi;
	GLuint rbo;
	GLuint screen_vao;
	GLuint screen_vbo;

	GLuint screen_texture;
	GLuint screen_texture_multi;

	GLubyte draw_mode = 0;
	GLubyte first_press = 0;

	GLdouble time_now;
	GLdouble time_last;
	GLdouble time_delta;
	GLdouble time_elapsed = 0.0;

	GLdouble fps_accumulated = 0.0f;
	GLdouble fps_timer = 0.0f;
	GLuint fps_count = 0;

	GLdouble mouse_x, mouse_y;

	player_t player;
	GLfloat trip_intensity = 0.0f;

	vec3 pistol_pos;
	vec2 pistol_angle;
	vec2 pistol_angle_tar;

	GLuint shader;
	GLuint light_shader;
	GLuint screen_shader;
	texture_t textures[2];
	texture_t room_textures[6];
	texture_t gun_textures[2];
	mesh_t mesh_cube;
	mesh_t mesh_light;

	model_t model_room;
	model_t model_pistol;

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
		2, 1, 0,
		3, 1, 2,
		4, 5, 6,
		6, 5, 7,
		10, 9, 8,
		11, 9, 10,
		12, 13, 14,
		14, 13, 15,
		18, 17, 16,
		19, 17, 18,
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

	GLfloat screen_vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
	};

	glm_vec3_copy(GLM_VEC3_ZERO, player.headbob);
	player.headbob_timer = 0.0f;
	player.headbob_intensity = 0.0f;
	glm_vec3_copy(GLM_VEC3_ONE, light_color);
	glm_vec3_scale(light_color, 0.5f, light_diffuse_color);
	glm_vec3_scale(light_diffuse_color, 0.2f, light_ambient_color);
	glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_view);

	/* camera initialization */
	glm_vec3_copy(GLM_YUP, player.up);
	glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, player.pos);
	glm_vec3_copy(player.pos, player.pos_end);
	glm_vec2_copy((vec2){0.0f, 0.0f}, player.rot);

	glm_vec3_copy(player.pos, pistol_pos);
	glm_vec3_add(pistol_pos, (vec3){cosf(glm_rad(player.rot[0])) * 0.3f, -0.22f, sinf(glm_rad(player.rot[0])) * 0.3f}, pistol_pos);
	glm_vec3_add(pistol_pos, (vec3){0.2f * sinf(glm_rad(-player.rot[0])), 0.0f, 0.2f * cosf(glm_rad(player.rot[0]))}, pistol_pos);

	glm_vec2_copy(GLM_VEC2_ZERO, pistol_angle);
	pistol_angle[0] = GLM_PIf;

	if(!glfwInit()) {
		printf("ERROR: GLFW fucked up.\n");
		return 1;
	}

	// glfwSetErrorCallback(error_callback);

	/* creating a window */
	{
		GLFWmonitor *monitor;
		const GLFWvidmode *mode;
		int count;
		monitor = *glfwGetMonitors(&count);
		mode = glfwGetVideoMode(monitor);
		window_width = mode->width * 0.75f;
		window_height = mode->height * 0.75f;
	}

	aspect_ratio = window_width / window_height;
	glm_perspective(14.0f, aspect_ratio, 0.1f, 1000.0f, matrix_projection);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(window_width, window_height, WINDOW_TITLE, NULL, NULL);
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

	glViewport(0, 0, window_width, window_height);

	/* loading shaders */
	shader = shader_create("res/shaders/vert.glsl", "res/shaders/frag.glsl");
	glUseProgram(shader);
	glUniform3f(glGetUniformLocation(shader, "material.specular_color"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(shader, "material.shininess"), 32.0f);

	glUniform3f(glGetUniformLocation(shader, "light_dir.dir"), 0.0f, 0.0f, -1.0f);
	glUniform3fv(glGetUniformLocation(shader, "light_dir.ambient_color"), 1, (const GLfloat *)light_ambient_color);
	glUniform3fv(glGetUniformLocation(shader, "light_dir.diffuse_color"), 1, (const GLfloat *)light_diffuse_color);
	glUniform3f(glGetUniformLocation(shader, "light_dir.specular_color"), 1.0f, 1.0f, 1.0f);

	for(GLuint i = 0; i < 4; i++) {
		char buffer[128] = { 0 };
		sprintf(buffer, "light_points[%d].pos", i);
		glUniform3fv(glGetUniformLocation(shader, buffer), 1, (const GLfloat *)light_point_positions[i]);
		sprintf(buffer, "light_points[%d].ambient_color", i);
		glUniform3fv(glGetUniformLocation(shader, buffer), 1, (const GLfloat *)light_ambient_color);
		sprintf(buffer, "light_points[%d].diffuse_color", i);
		glUniform3fv(glGetUniformLocation(shader, buffer), 1, (const GLfloat *)light_diffuse_color);
		sprintf(buffer, "light_points[%d].specular_color", i);
		glUniform3f(glGetUniformLocation(shader, buffer), 1.0f, 1.0f, 1.0f);
		sprintf(buffer, "light_points[%d].constant", i);
		glUniform1f(glGetUniformLocation(shader, buffer), 1.0f);
		sprintf(buffer, "light_points[%d].linear", i);
		glUniform1f(glGetUniformLocation(shader, buffer), 0.09f);
		sprintf(buffer, "light_points[%d].quadratic", i);
		glUniform1f(glGetUniformLocation(shader, buffer), 0.032f);
	}

	light_shader = shader_create("res/shaders/light-vert.glsl", "res/shaders/light-frag.glsl");
	glUseProgram(light_shader);

	/* loading textures */
	textures[0] = texture_create("res/textures/box-diffuse.png", TT_DIFFUSE);
	textures[1] = texture_create("res/textures/box-specular.png", TT_SPECULAR);

	room_textures[0] = texture_create("res/models/room/floor-diffuse.png", TT_DIFFUSE);
	room_textures[1] = texture_create("res/models/room/black.png", TT_SPECULAR);
	room_textures[2] = texture_create("res/models/room/wall-diffuse.png", TT_DIFFUSE);
	room_textures[3] = room_textures[1];
	room_textures[4] = texture_create("res/models/room/ceiling-diffuse.png", TT_DIFFUSE);
	room_textures[5] = texture_create("res/models/room/ceiling-specular.png", TT_SPECULAR);

	gun_textures[0] = room_textures[4];
	gun_textures[1] = room_textures[1];

	/* loading meshes */
	model_room = model_create("res/models/room/room.glb", room_textures, 6);
	model_pistol = model_create("res/models/weapons/pistol/pistol.glb", gun_textures, 2);
	mesh_cube = mesh_create(vertices, indices, textures, sizeof(vertices) / sizeof(vertex_t), sizeof(indices) / sizeof(GLuint), 2);
	mesh_light = mesh_create(vertices, indices, NULL, sizeof(vertices) / sizeof(vertex_t), sizeof(indices) / sizeof(GLuint), 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_MULTISAMPLE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, (double)window_width / 2, (double)window_height / 2);

	time_now = glfwGetTime();
	time_last = time_now;

	/* getting the multisample framebuffer set up */
	glGenFramebuffers(1, &fbo_multi);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_multi);

	glGenTextures(1, &screen_texture_multi);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, screen_texture_multi);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample_count, GL_RGB, window_width, window_height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, screen_texture_multi, 0);

	/* getting the renderbuffer set up */
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample_count, GL_DEPTH24_STENCIL8, window_width, window_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Multisampled framebuffer fucked up.\n");
		return 1;
	}

	/* getting the regular framebuffer set up */
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &screen_texture);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Main framebuffer fucked up.\n");
		return 1;
	}

	/* binding the render buffer to a quad */
	screen_shader = shader_create("res/shaders/screen-vert.glsl", "res/shaders/screen-frag.glsl");
	glUseProgram(screen_shader);

	glGenVertexArrays(1, &screen_vao);
	glBindVertexArray(screen_vao);

	glGenBuffers(1, &screen_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vertex_t), screen_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	/* main loop */
	while(!glfwWindowShouldClose(window)) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);

		time_now = glfwGetTime();
		time_delta = time_now - time_last;
		time_last = time_now;
		time_elapsed += time_delta;

		fps_timer += time_delta;
		if(fps_timer >= 1.0f) {
			fps_accumulated = 0.0f;
			fps_count = 0;
			fps_timer = 0.0f;
		}

		fps_accumulated += 1.0 / time_delta;
		fps_count++;

		printf("FPS: %f\n", fps_accumulated / fps_count);

		/* camera look */
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		mouse_x -= (double)window_width / 2;
		mouse_y -= (double)window_height / 2;

		/* TODO: Get mouse button for shit */
		// printf("%d\n", glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1));

		player.rot[0] += mouse_x * CAM_SENSITIVITY;
		player.rot[1] -= mouse_y * CAM_SENSITIVITY;
		if(player.rot[1] > 89.0f)
			player.rot[1] = 89.0f;

		if(player.rot[1] < -89.0f)
			player.rot[1] = -89.0f;

		/* if(player.rot[0] < -180.0f)
			player.rot[0] = 180.0f;

		if(player.rot[0] > 180.0f)
			player.rot[0] = -180.0f; */

		player.dir[0] = cos(glm_rad(player.rot[0])) * cos(glm_rad(player.rot[1]));
		player.dir[1] = sin(glm_rad(player.rot[1]));
		player.dir[2] = sin(glm_rad(player.rot[0])) * cos(glm_rad(player.rot[1]));
		glm_normalize(player.dir);

		glm_cross(player.dir, player.up, player.right);
		glm_normalize(player.right);

		glfwSetCursorPos(window, (double)window_width / 2, (double)window_height / 2);

		/* toggling wireframe */
		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !first_press) {
			draw_mode = !draw_mode;
			first_press = 1;
		}

		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
			first_press = 0;

		{ /* handing player movement */
			float player_speed = (float)time_delta * PLAYER_MOVE_SPEED;
			vec3 player_move;

			GLubyte any_key_pressed = 0;
			GLubyte keys[5] = {
				glfwGetKey(window, GLFW_KEY_A),
				glfwGetKey(window, GLFW_KEY_D),
				glfwGetKey(window, GLFW_KEY_W),
				glfwGetKey(window, GLFW_KEY_S),
				glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
			};

			player_speed += keys[4] * PLAYER_MOVE_SPEED * time_delta;

			glm_vec3_copy(GLM_VEC3_ZERO, player_move);
			if(keys[0]) {
				glm_vec3_sub(player_move, player.right, player_move);
			}

			if(keys[1]) {
				glm_vec3_add(player_move, player.right, player_move);
			}

			if(keys[2]) {
				glm_vec3_add(player_move, player.dir, player_move);
			}

			if(keys[3]) {
				glm_vec3_sub(player_move, player.dir, player_move);
			}

			for(GLubyte i = 0; i < 4; i++) {
				any_key_pressed += keys[i];
			}

			player.headbob_intensity = glm_lerp(player.headbob_intensity, (float)(any_key_pressed > 0), time_delta * HEADBOB_LERP_SPEED);

			player_move[1] = 0.0f;
			glm_vec3_normalize(player_move);
			glm_vec3_scale(player_move, player_speed, player_move);
			glm_vec3_add(player.pos_end, player_move, player.pos_end);

			player.headbob_timer += time_delta * player.headbob_intensity * ((float)(keys[4] + 1) * 0.75f);
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
		}

		glm_mat4_copy(GLM_MAT4_IDENTITY, matrix_view);

		player.headbob[0] = cosf(player.headbob_timer * GLM_PIf * 2) * HEADBOB_INTENSITY;
		player.headbob[1] = sinf(player.headbob_timer * GLM_PIf * 4) * HEADBOB_INTENSITY * 0.66f;
		player.headbob[2] = cosf(player.headbob_timer * GLM_PIf * 2) * HEADBOB_INTENSITY;

		player.headbob[0] *= -sinf(glm_rad(player.rot[0]));
		player.headbob[2] *= cosf(glm_rad(player.rot[0]));
		glm_vec3_scale(player.headbob, player.headbob_intensity, player.headbob);

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

		glBindFramebuffer(GL_FRAMEBUFFER, fbo_multi);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// trip_intensity += time_delta * 0.12f; // fabsf((0.5f - trip_intensity) * time_delta);
		if(trip_intensity > 1.0f)
			trip_intensity = 1.0f;

		glm_perspective(((cosf(time_elapsed * 3.14f) + 1.0f) * trip_intensity * 0.04f) + 14.0f, ((sinf(time_elapsed * 3.14f) + 1.0f) * trip_intensity * 0.04f) + aspect_ratio, 0.1f, 1000.0f, matrix_projection);

		glUseProgram(shader);
		glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, (const GLfloat *)matrix_view);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, (const GLfloat *)matrix_projection);
		glUniform3fv(glGetUniformLocation(shader, "view_pos"), 1, (const GLfloat *)player.pos);
		glUniform1f(glGetUniformLocation(shader, "time"), time_elapsed);
		glUniform1f(glGetUniformLocation(shader, "trip_intensity"), trip_intensity);
		glUniform1i(glGetUniformLocation(shader, "render_layer"), 0);

		for(GLuint i = 0; i < 10; i++) {
			mat4 model_mat;
			float angle = 20 * i + (time_elapsed * GLM_PI * 12);
			glm_mat4_copy(GLM_MAT4_IDENTITY, model_mat);
			glm_translate(model_mat, cube_positions[i]);
			glm_rotate(model_mat, glm_rad(angle), (vec3){1.0f, 0.3f, 0.5f});
			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (const GLfloat *)model_mat);
			// mesh_draw(mesh_cube, shader);
		}

		{ /* drawing room */
			mat4 model_room_mat;
			glm_mat4_copy(GLM_MAT4_IDENTITY, model_room_mat);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[0].id);
			glUniform1i(glGetUniformLocation(shader, "material.tex_diffuse0"), 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures[1].id);
			glUniform1i(glGetUniformLocation(shader, "material.tex_specular0"), 1);

			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (const GLfloat *)model_room_mat);
			model_draw(model_room, shader);
		}

		glUniform1i(glGetUniformLocation(shader, "render_layer"), 1);

		{ /* drawing pistol */
			vec3 pistol_tar_pos;
			vec3 pistol_bob;
			mat4 model_pistol_mat;

			/* pistol position */
			glm_vec3_copy(player.pos_bobbed, pistol_tar_pos);
			glm_vec3_add(pistol_tar_pos, (vec3){cosf(glm_rad(player.rot[0])) * 0.3f, 0.22f * player.dir[1] - 0.22f, sinf(glm_rad(player.rot[0])) * 0.3f}, pistol_tar_pos);
			glm_vec3_add(pistol_tar_pos, (vec3){0.2f * sinf(glm_rad(-player.rot[0])), 0.0f, 0.2f * cosf(glm_rad(player.rot[0]))}, pistol_tar_pos);
			glm_vec3_lerp(pistol_pos, pistol_tar_pos, time_delta * 48.0f, pistol_pos);

			glm_mat4_copy(GLM_MAT4_IDENTITY, model_pistol_mat);
			glm_translate(model_pistol_mat, pistol_pos);

			/* apply headbob */
			glm_vec3_copy(player.headbob, pistol_bob);
			glm_vec3_scale(pistol_bob, -0.3f, pistol_bob);
			glm_translate(model_pistol_mat, pistol_bob);

			/* pistol rotation */
			pistol_angle_tar[0] = glm_rad(-player.rot[0] + 180.0f);
			pistol_angle[0] = glm_lerp(pistol_angle[0], pistol_angle_tar[0], time_delta * 16.0f);
			glm_rotate(model_pistol_mat, pistol_angle[0], player.up);

			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (const GLfloat *)model_pistol_mat);
			model_draw(model_pistol, shader);
		}

		glUseProgram(light_shader);
		glUniformMatrix4fv(glGetUniformLocation(light_shader, "view"), 1, GL_FALSE, (const GLfloat *)matrix_view);
		glUniformMatrix4fv(glGetUniformLocation(light_shader, "projection"), 1, GL_FALSE, (const GLfloat *)matrix_projection);
		glUniform3fv(glGetUniformLocation(light_shader, "light_color"), 1, (const GLfloat *)light_color);
		for(GLuint i = 0; i < 4; i++) {
			mat4 light_mat;
			glm_mat4_copy(GLM_MAT4_IDENTITY, light_mat);
			glm_translate(light_mat, light_point_positions[i]);
			glm_scale(light_mat, (vec3){0.2f, 0.2f, 0.2f});
			glUniformMatrix4fv(glGetUniformLocation(light_shader, "model"), 1, GL_FALSE, (const GLfloat *)light_mat);
			mesh_draw(mesh_light, light_shader);
		}

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_multi);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(screen_shader);
		glUniform1i(glGetUniformLocation(screen_shader, "screen_texture"), 1);
		glUniform1f(glGetUniformLocation(screen_shader, "time"), time_elapsed);
		glUniform1f(glGetUniformLocation(screen_shader, "trip_intensity"), trip_intensity);
		glBindTexture(GL_TEXTURE_2D, screen_texture);

		glBindVertexArray(screen_vao);
		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);

	model_destroy(&model_pistol);
	model_destroy(&model_room);
	mesh_destroy(&mesh_light);
	mesh_destroy(&mesh_cube);
	texture_destroy(&textures[1]);
	texture_destroy(&textures[0]);
	glDeleteShader(screen_shader);
	glDeleteShader(light_shader);
	glDeleteShader(shader);

	glfwTerminate();

	return 0;
}
