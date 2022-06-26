#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

typedef struct {
	vec2 rot;
	vec3 pos;
	vec3 pos_end;
	vec3 dir;
	vec3 tar;
	vec3 up;
	vec3 right;
} camera_t;

#endif
