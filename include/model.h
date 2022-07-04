#ifndef MODEL_H
#define MODEL_H

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

typedef struct {
	mesh_t *meshes;
	GLuint mesh_count;
} model_t;

model_t model_create(const char *path, texture_t *textures, const GLuint texture_count);
void model_process_node(model_t *m, struct aiNode *node, const struct aiScene *scene, texture_t *textures, const GLuint texture_count);
void model_draw(model_t m, GLuint s);
void model_destroy(model_t *m);

#endif
