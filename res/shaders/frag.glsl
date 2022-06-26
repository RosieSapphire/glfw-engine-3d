#version 330 core

in vec3 normal;
in vec2 uv;
in vec3 frag_pos;

out vec4 FragColor;

uniform vec3 view_pos;

struct material_t {
	sampler2D diffuse_tex;
	sampler2D specular_tex;
	float shininess;
}; uniform material_t material;

struct light_t {
	vec3 pos;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
}; uniform light_t light;

void main() {
	vec3 tex_color = vec3(texture(material.diffuse_tex, uv));
	vec3 ambient_color = light.ambient_color * tex_color;

	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light.pos - frag_pos);
	float diffuse_amount = max(dot(norm, light_dir), 0.0);
	vec3 diffuse_color = light.diffuse_color * diffuse_amount * tex_color;

	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float specular_amount = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular_color = light.specular_color * vec3(specular_amount * texture(material.specular_tex, uv).r);

	vec3 final_color = ambient_color + diffuse_color + specular_color;
	FragColor = vec4(final_color, 1.0);
}
