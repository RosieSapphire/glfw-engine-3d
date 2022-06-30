#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

typedef struct {
	mesh_t *meshes;
	GLuint mesh_count;
} model_t;

model_t model_create(const char *path);
void model_draw(model_t m, GLuint s);

#endif
