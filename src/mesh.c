#include "mesh.h"

#include <stdlib.h>

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

	for(i = 0; i < index_count; i++) {
		m.indices[i] = indices[i];
		printf("%d\n", indices[i]);
	}

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

	return m;
}

void mesh_destroy(mesh_t *m) {
	free(m->vertices);
	free(m->indices);
	free(m->textures);
}
