#version 330 core

#define POINT_LIGHT_COUNT	4

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

struct light_point_t {
	vec3 pos;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	float constant;
	float linear;
	float quadratic;
}; uniform light_point_t light_points[POINT_LIGHT_COUNT];

vec3 get_light_point(light_point_t l, vec3 normal, vec3 frag_pos, vec3 view_dir) {
	vec3 light_dir = normalize(l.pos - frag_pos);
	float diffuse_amount = max(dot(normal, light_dir), 0.0);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float specular_amount = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	float distance = length(l.pos - frag_pos);
	float attenuation = 1.0 / (l.constant + l.linear * distance + l.quadratic * (distance * distance));

	vec3 ambient_color = l.ambient_color * vec3(texture(material.diffuse_tex, uv));
	vec3 diffuse_color = l.diffuse_color * diffuse_amount * vec3(texture(material.diffuse_tex, uv));
	vec3 specular_color = l.specular_color * specular_amount * vec3(texture(material.specular_tex, uv).r);
	ambient_color *= attenuation;
	diffuse_color *= attenuation;
	specular_color *= attenuation;
	return (ambient_color + diffuse_color + specular_color);
}

struct light_dir_t {
	vec3 dir;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
}; uniform light_dir_t light_dir;

vec3 get_light_dir(light_dir_t l, vec3 normal, vec3 view_dir) {
	vec3 light_dir = normalize(-l.dir);
	float diffuse_amount = max(dot(normal, light_dir), 0.0);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float specular_amount = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

	vec3 ambient_color = l.ambient_color * vec3(texture(material.diffuse_tex, uv));
	vec3 diffuse_color = l.diffuse_color * diffuse_amount * vec3(texture(material.diffuse_tex, uv));
	vec3 specular_color = l.specular_color * specular_amount * vec3(texture(material.specular_tex, uv).r);
	return (ambient_color + diffuse_color + specular_color);
}

void main() {
	vec3 norm = normalize(normal);
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 final_color = get_light_dir(light_dir, norm, view_dir);

	for(int i = 0; i < POINT_LIGHT_COUNT; i++)
		final_color += get_light_point(light_points[i], norm, frag_pos, view_dir);

	FragColor = vec4(final_color, 1.0);
}
