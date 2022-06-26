#version 330 core

in vec3 vert_color;
in vec3 normal;
in vec2 uv;
in vec3 frag_pos;

out vec4 FragColor;

uniform sampler2D tex01;
uniform vec3 view_pos;

struct material_t {
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	float shininess;
}; uniform material_t material;

struct light_t {
	vec3 pos;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
}; uniform light_t light;

void main() {
	vec3 ambient_color = light.ambient_color * material.ambient_color;

	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light.pos - frag_pos);
	float diffuse_amount = max(dot(norm, light_dir), 0.0);
	vec3 diffuse_color = light.diffuse_color * (diffuse_amount * material.diffuse_color);

	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float specular_amount = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular_color = light.specular_color * (specular_amount * material.specular_color);

	vec4 tex_color = texture(tex01, uv);
	// vec3 uv_tex_color = vec3(tex_color) * vert_color;
	vec3 uv_tex_color = vec3(1.0f, 0.5f, 0.25f);

	vec3 final_color = uv_tex_color * (ambient_color + diffuse_color + specular_color);
	FragColor = vec4(final_color, 1.0);
}
