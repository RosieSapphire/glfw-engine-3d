#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

texture_t texture_create(const char *path, const texture_type_t type) {
	texture_t t;
	GLubyte *data;
	glGenTextures(1, &t.id);
	glBindTexture(GL_TEXTURE_2D, t.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	t.type = type;

	stbi_set_flip_vertically_on_load(1);
	data = stbi_load(path, &t.width, &t.height, &t.channels, 0);
	if(!data) {
		switch(type) {
			case TT_DIFFUSE:
				printf("ERROR: Diffuse texture fucked up.\n");
				break;

			case TT_SPECULAR:
				printf("ERROR: Specular texture fucked up.\n");
				break;

			default:
				printf("ERROR: Unspecified texture fucked up.\n");
				break;
		}

		glfwTerminate();
		return t;
	}

	switch(t.channels) {
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, t.width, t.height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			break;

		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t.width, t.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;

		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width, t.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	return t;
}

void texture_destroy(texture_t *t) {
	glDeleteTextures(1, &t->id);
}
