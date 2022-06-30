#include "model.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

model_t model_create(const char *path) {
	GLuint i, j;
	model_t m;
	const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("ERROR: %s\n", aiGetErrorString());
		return m;
	}

	/* TODO: I am too tired to work on this right now. */
	m.meshes = malloc(scene->mRootNode->mNumMeshes * sizeof(mesh_t));
	for(i = 0; i < scene->mRootNode->mNumMeshes; i++) {
		struct aiMesh *mesh = scene->mMeshes[scene->mRootNode->mMeshes[i]];
		for(j = 0; j < mesh->mNumVertices; j++) {
			vertex_t *const vert_cur = &m.meshes[i].vertices[j];
			vert_cur->pos[0] = mesh->mVertices[j].x;
			vert_cur->pos[1] = mesh->mVertices[j].y;
			vert_cur->pos[2] = mesh->mVertices[j].z;
			vert_cur->normal[0] = mesh->mNormals[j].x;
			vert_cur->normal[1] = mesh->mNormals[j].y;
			vert_cur->normal[2] = mesh->mNormals[j].z;
			vert_cur->uv[0] = mesh->mTextureCoords[j]->x;
			vert_cur->uv[1] = mesh->mTextureCoords[j]->y;
		}
	}

	return m;
}

void model_draw(model_t m, GLuint s) {
	for(GLuint i = 0; i < m.mesh_count; i++)
		mesh_draw(m.meshes[i], s);
}
