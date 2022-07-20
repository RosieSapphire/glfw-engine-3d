#include "model.h"

model_t model_create(const char *path, texture_t *textures, const GLuint texture_count) {
	const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_RemoveRedundantMaterials);
	model_t m;
	m.meshes = NULL;
	m.mesh_count = 0;

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("ERROR: %s\n", aiGetErrorString());
		m.mesh_count = 0;
		return m;
	}

	model_process_node(&m, scene->mRootNode, scene, textures, texture_count);

	return m;
}

void model_process_node(model_t *m, struct aiNode *node, const struct aiScene *scene, texture_t *textures, const GLuint texture_count) {
	GLuint i, j, k;

	if(!m->mesh_count) {
		m->mesh_count = node->mNumMeshes;
		m->meshes = malloc(m->mesh_count * sizeof(mesh_t));
	} else {
		m->mesh_count += node->mNumMeshes;
		m->meshes = realloc(m->meshes, m->mesh_count * sizeof(mesh_t));
	}

	for(i = m->mesh_count - node->mNumMeshes; i < m->mesh_count; i++) {
		const struct aiMesh *const mesh = scene->mMeshes[node->mMeshes[i]];
		vertex_t *vertices = NULL;
		GLuint *indices = NULL;
		texture_t selected_textures[2];

		GLuint texture_count = 0;
		GLuint index_count = 0;
		// GLuint bone_count = 0;

		vertices = malloc(mesh->mNumVertices * sizeof(vertex_t));
		for(j = 0; j < mesh->mNumVertices; j++) {
			vertices[j].pos[0] = mesh->mVertices[j].x;
			vertices[j].pos[1] = mesh->mVertices[j].y;
			vertices[j].pos[2] = mesh->mVertices[j].z;
			vertices[j].normal[0] = mesh->mNormals[j].x;
			vertices[j].normal[1] = mesh->mNormals[j].y;
			vertices[j].normal[2] = mesh->mNormals[j].z;

			if(mesh->mTextureCoords[0]) {
				vertices[j].uv[0] = mesh->mTextureCoords[0][j].x;
				vertices[j].uv[1] = mesh->mTextureCoords[0][j].y;
			} else {
				glm_vec2_copy(GLM_VEC2_ZERO, vertices[j].uv);
			}
		}

		indices = malloc(mesh->mNumFaces * 3 * sizeof(GLuint));
		for(j = 0; j < mesh->mNumFaces; j++) {
			const struct aiFace face = mesh->mFaces[j];
			for(k = index_count; k < index_count + face.mNumIndices; k++) {
				indices[k] = face.mIndices[k - index_count];
			}

			index_count = k;
		}

		if(textures) {
			selected_textures[0] = textures[0 + (i * 2)];
			selected_textures[1] = textures[1 + (i * 2)];
			texture_count = 2;
		}

		/* TODO: Replace the bone code with something better here */

		m->meshes[i] = mesh_create(vertices, indices, selected_textures, mesh->mNumVertices, mesh->mNumFaces * 3, texture_count);
		free(indices);
		free(vertices);
		indices = NULL;
		vertices = NULL;
	}

	for(i = 0; i < node->mNumChildren; i++) {
		model_process_node(m, node->mChildren[i], scene, textures, texture_count);
	}
}

void model_draw(model_t m, GLuint s) {
	for(GLuint i = 0; i < m.mesh_count; i++)
		mesh_draw(m.meshes[i], s);
}

void model_destroy(model_t *m) {
	free(m->meshes);
}
