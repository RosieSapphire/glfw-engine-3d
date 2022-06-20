#version 330 core

in vec4 color;
in vec2 uv;

out vec4 FragColor;

uniform sampler2D tex01;
uniform sampler2D tex02;

uniform float lerp;

void main() {
	FragColor = mix(texture(tex01, uv), texture(tex02, uv), lerp) * color;
}
