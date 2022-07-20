#version 330 core

out vec4 FragColor;

in vec2 uvs;
in vec4 frag_pos;

uniform sampler2D screen_texture;
uniform float time;
uniform float trip_intensity;

void main() {
	FragColor = texture(screen_texture, vec2(uvs.x + ((sin(uvs.y * 4 + time) / 64) * trip_intensity), uvs.y + ((cos(uvs.x * 4 + time) / 64) * trip_intensity)));
	// FragColor *= vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
