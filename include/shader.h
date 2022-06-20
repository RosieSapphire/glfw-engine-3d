#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLuint shader_create(const char *vert_path, const char *frag_path);

#endif
