#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec2 a_uv;

out vec3 vert_color;
out vec3 normal;
out vec2 uv;
out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(a_pos, 1.0);

	vert_color = a_color;
	normal = a_normal;
	uv = a_uv;
	frag_pos = vec3(model * vec4(a_pos, 1.0));
}
