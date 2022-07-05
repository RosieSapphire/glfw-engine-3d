#ifndef PLAYER_H
#define PLAYER_H

#include <cglm/cglm.h>

typedef struct {
	vec3 up;
	vec3 right;
	vec2 rot;
	vec3 pos;
	vec3 pos_bobbed;
	vec3 pos_end;
	vec3 tar;
	vec3 tar_bobbed;
	vec3 dir;

	vec3 headbob;
	float headbob_timer;
	float headbob_intensity;
} player_t;

#endif
