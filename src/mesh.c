#include "mesh.h"

#include <stdlib.h>
#include <string.h>

mesh_t mesh_create(vertex_t *vertices, GLuint *indices, texture_t *textures, const GLuint vertex_count, const GLuint index_count, const GLuint texture_count) {
	GLuint i;
	mesh_t m;
	
	m.vertices = malloc(vertex_count * sizeof(vertex_t));
	m.indices = malloc(index_count * sizeof(GLuint));
	m.textures = malloc(texture_count * sizeof(texture_t));

	for(i = 0; i < vertex_count; i++) {
		glm_vec3_copy(vertices[i].pos, m.vertices[i].pos);
		glm_vec3_copy(vertices[i].normal, m.vertices[i].normal);
		glm_vec2_copy(vertices[i].uv, m.vertices[i].uv);
	}

	for(i = 0; i < index_count; i++)
		m.indices[i] = indices[i];

	for(i = 0; i < texture_count; i++)
		m.textures[i] = textures[i];

	glGenVertexArrays(1, &m.vao);
	glBindVertexArray(m.vao);

	glGenBuffers(1, &m.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(vertex_t), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(GLuint), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	m.vertex_count = vertex_count;
	m.index_count = index_count;
	m.texture_count = texture_count;

	return m;
}

void mesh_draw(mesh_t m, GLuint s) {
	GLuint diffuse_count = 0;
	GLuint specular_count = 0;
	GLuint i;

	for(i = 0; i < m.texture_count; i++) {
		const char *name;
		char uniform[32];
		char num[2];
		memset(uniform, 0, sizeof(uniform));
		memset(num, 0, sizeof(num));
		glActiveTexture(GL_TEXTURE0 + i);

		switch(m.textures[i].type) {
			case TT_DIFFUSE:
				name = "tex_diffuse";
				*num = 48 + diffuse_count++;
				break;

			case TT_SPECULAR:
				name = "tex_specular";
				*num = 48 + specular_count++;
				break;

			default:
				name = "unknown";
				printf("ERROR: Unknown texture type");
				glfwTerminate();
				return;
		}

		strcpy(uniform, "material.");

		glUniform1i(glGetUniformLocation(s, strcat(strcat(uniform, name), num)), i);
		glBindTexture(GL_TEXTURE_2D, m.textures[i].id);
	}

	glBindVertexArray(m.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
	glDrawElements(GL_TRIANGLES, m.index_count, GL_UNSIGNED_INT, 0);
}

void mesh_destroy(mesh_t *m) {
	free(m->vertices);
	free(m->indices);
	free(m->textures);
}
