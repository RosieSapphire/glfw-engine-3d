#version 330 core

in vec3 vert_color;
in vec3 normal;
in vec2 uv;
in vec3 frag_pos;

out vec4 FragColor;

uniform sampler2D tex01;

uniform vec3 light_color;
uniform vec3 light_pos;

uniform vec3 view_pos;

void main() {
	float ambient_amount = 0.1f;
	vec3 ambient_color = ambient_amount * light_color;

	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light_pos - frag_pos);
	float diffuse_amount = max(dot(norm, light_dir), 0.0);
	vec3 diffuse_color = diffuse_amount * light_color;

	float specular_strength = 0.5f;
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float specular_amount = pow(max(dot(view_dir, reflect_dir), 0.0), 16);
	vec3 specular_color = specular_strength * specular_amount * light_color;

	vec4 tex_color = texture(tex01, uv);
	// vec3 uv_tex_color = vec3(tex_color) * vert_color;
	vec3 uv_tex_color = vec3(1.0f, 0.5f, 0.25f);

	vec3 final_color = uv_tex_color * (ambient_color + diffuse_color + specular_color);
	FragColor = vec4(final_color, 1.0);
}
