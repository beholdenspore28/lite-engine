#include "lite_engine.h"

ui64        *internal_ECS_data                 = NULL;
ui64         internal_ECS_entity_count         = 0;
ui64         internal_ECS_entity_capacity      = 1024;
enum { INTERNAL_ECS_ENTITY_GROWTH_FACTOR = 2 };

ui64 lite_engine_ECS_get_capacity(void) {
	return internal_ECS_entity_capacity;	
}

void lite_engine_ECS_start(void) {
	debug_log("starting ECS...");
	internal_ECS_data = calloc(
			sizeof(*internal_ECS_data),
			internal_ECS_entity_capacity);
}

void lite_engine_ECS_stop(void) {
	debug_log("ECS shutting down...");

	free(internal_ECS_data);
}

ui64 lite_engine_ECS_entity_create(void) {
	internal_ECS_entity_count++;
	if (internal_ECS_entity_count >= internal_ECS_entity_capacity) {
		internal_ECS_entity_capacity *= INTERNAL_ECS_ENTITY_GROWTH_FACTOR;
		internal_ECS_data = realloc(
				internal_ECS_data,
				sizeof(*internal_ECS_data) * internal_ECS_entity_capacity);
	}
	return internal_ECS_entity_count;
}
