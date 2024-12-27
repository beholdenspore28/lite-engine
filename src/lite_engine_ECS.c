#include "lite_engine.h"

int        *internal_ECS_data                 = NULL;
int         internal_ECS_entity_count         = 0;
int         internal_ECS_entity_capacity      = 1024;
const int   internal_ECS_entity_growth_factor = 2;

void lite_engine_ECS_start(void) {
	internal_ECS_data = calloc(sizeof(int), internal_ECS_entity_capacity);
}

void lite_engine_ECS_stop(void) {
	free(internal_ECS_data);
}

int lite_engine_ECS_entity_create(void) {
	internal_ECS_entity_count++;
	assert(internal_ECS_entity_count < internal_ECS_entity_capacity);
	return internal_ECS_entity_count;
}
