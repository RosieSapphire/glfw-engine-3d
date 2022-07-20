#ifndef BONE_H
#define BONE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	unsigned int *bone_ids;
	float *weights;
	unsigned int bone_count;
} bone_data_t;

void bone_data_create(bone_data_t *data, const unsigned int bone_count) {
	data->bone_ids = malloc(bone_count * sizeof(unsigned int));
	memset(data->bone_ids, 0, bone_count);
	/* TODO: malloc the fucking weights */
}

void bone_data_destroy(bone_data_t *data) {
	free(data->bone_ids);
	free(data->weights);
}

void bone_add_data(bone_data_t *data, const unsigned int bone_id, const float weight) {
	for(unsigned int i = 0; i < data->bone_count; i++) {
		if(!data->weights[i]) {
			data->bone_ids[i] = bone_id;
			data->weights[i] = weight;
			printf("Bone: %d, Weight: %f, Index: %d\n", bone_id, weight, i);
		}
	}
}

#endif
