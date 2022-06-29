#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include "vertex.h"
#include "texture.h"
#include "camera.h"

typedef struct {
	vertex_t *vertices;
	GLuint *indices;
	texture_t *textures;

	GLuint vao, vbo, ebo;
	GLuint vertex_count, index_count, texture_count;
} mesh_t;

mesh_t mesh_create(vertex_t *vertices, GLuint *indices, texture_t *textures, const GLuint vertex_count, const GLuint index_count, const GLuint texture_count);
void mesh_draw(mesh_t m);
void mesh_destroy(mesh_t *m);

#endif
