#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef enum {
	TT_DIFFUSE,
	TT_SPECULAR,
} texture_type_t;

typedef struct {
	GLuint id;
	GLint width, height, channels;
	texture_type_t type;
} texture_t;

texture_t texture_create(const char *path, const texture_type_t type);
void texture_destroy(texture_t *t);

#endif
