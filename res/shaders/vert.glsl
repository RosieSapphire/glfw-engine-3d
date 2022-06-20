#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec2 a_uv;

out vec4 color;
out vec2 uv;

void main() {
	gl_Position = vec4(a_pos, 1.0);

	color = vec4(a_color, 1.0);
	uv = a_uv;
}
