#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include "vertex.h"
#include "texture.h"

typedef struct {
	vertex_t *vertices;
	GLuint *indices;
	texture_t *textures;

	GLuint vao, vbo, ebo;
} mesh_t;

mesh_t mesh_create(vertex_t *vertices, GLuint *indices, texture_t *textures, const GLuint vertex_count, const GLuint index_count, const GLuint texture_count);
void mesh_destroy(mesh_t *m);

#endif
