#version 330 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uvs;

out vec2 uvs;
out vec4 frag_pos;

void main() {
	gl_Position = vec4(vec2(a_pos.xy), 0.0, 1.0);
	uvs = a_uvs;
	frag_pos = gl_Position;
}
