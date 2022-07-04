#include "model.h"

model_t model_create(const char *path) {
	const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_RemoveRedundantMaterials);
	model_t m;
	m.meshes = NULL;
	m.mesh_count = 0;

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("ERROR: %s\n", aiGetErrorString());
		m.mesh_count = 0;
		return m;
	}

	model_process_node(&m, scene->mRootNode, scene);

	return m;
}

void model_process_node(model_t *m, struct aiNode *node, const struct aiScene *scene) {
	GLuint i;

	if(!m->mesh_count) {
		m->mesh_count = node->mNumMeshes;
		m->meshes = malloc(m->mesh_count * sizeof(mesh_t));
	} else {
		m->mesh_count += node->mNumMeshes;
		m->meshes = realloc(m->meshes, m->mesh_count * sizeof(mesh_t));
	}

	for(i = m->mesh_count - node->mNumMeshes; i < m->mesh_count; i++) {
		const struct aiMesh *const mesh = scene->mMeshes[node->mMeshes[i]];
		const struct aiMaterial *const mat = scene->mMaterials[mesh->mMaterialIndex];
		GLuint j;
		vertex_t *vertices = NULL;
		GLuint *indices = NULL;
		texture_t *textures = NULL;

		GLuint indices_counted = 0;
		GLuint textures_loaded = 0;
		GLuint diffuse_count = aiGetMaterialTextureCount(mat, aiTextureType_DIFFUSE);
		GLuint specular_count = aiGetMaterialTextureCount(mat, aiTextureType_SPECULAR);

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
			GLuint k;
			const struct aiFace face = mesh->mFaces[j];
			for(k = indices_counted; k < indices_counted + face.mNumIndices; k++) {
				indices[k] = face.mIndices[k - indices_counted];
			}

			indices_counted = k;
		}

		textures = malloc((diffuse_count + specular_count) * sizeof(texture_t));
		for(j = 0; j < diffuse_count; j++) {
			struct aiString path;
			aiGetMaterialTexture(mat, aiTextureType_DIFFUSE, j, &path, NULL, NULL, NULL, NULL, NULL, NULL);
			textures[textures_loaded++] = texture_create(path.data, TT_DIFFUSE);
		}

		for(j = 0; j < specular_count; j++) {
			struct aiString path;
			aiGetMaterialTexture(mat, aiTextureType_SPECULAR, j, &path, NULL, NULL, NULL, NULL, NULL, NULL);
			textures[textures_loaded++] = texture_create(path.data, TT_SPECULAR);
		}
		printf("%d, %d\n", diffuse_count, specular_count);

		m->meshes[i] = mesh_create(vertices, indices, textures, mesh->mNumVertices, mesh->mNumFaces * 3, textures_loaded);
		free(textures);
		free(indices);
		free(vertices);
		indices = NULL;
		vertices = NULL;
	}

	for(i = 0; i < node->mNumChildren; i++) {
		model_process_node(m, node->mChildren[i], scene);
	}
}

void model_draw(model_t m, GLuint s) {
	for(GLuint i = 0; i < m.mesh_count; i++)
		mesh_draw(m.meshes[i], s);
}

void model_destroy(model_t *m) {
	free(m->meshes);
}
