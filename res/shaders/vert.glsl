#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

out vec3 normal;
out vec2 uv;
out vec3 frag_pos;

out float o_time;
out float o_trip_intensity;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float trip_intensity;
uniform int render_layer;

float rand(float n) {
	return fract(sin(n) * 43758.5453123);
}

float perlin_noise(float p){
	float fl = floor(p);
	float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

void main() {
	gl_Position = projection * view * model * vec4(a_pos, 1.0);

	vec2 trip_offset;
	trip_offset.x = perlin_noise((time * 1.57) + gl_Position.y) * (trip_intensity / 5);
	trip_offset.y = cos(time * 0.785 + gl_Position.z) * (trip_intensity / 5);
	gl_Position += vec4(trip_offset, 0.0, 0.0) * float(render_layer == 0);

	normal = mat3(transpose(inverse(model))) * a_normal;
	uv = a_uv;
	frag_pos = vec3(model * vec4(a_pos, 1.0));

	o_time = time;
	o_trip_intensity = trip_intensity;
}
