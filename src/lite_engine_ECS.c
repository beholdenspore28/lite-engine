#include "lite_engine.h"

static ui64   internal_ECS_entities_count      = 0;

static ui64 **internal_ECS_components          = NULL;
static ui64   internal_ECS_components_count    = 0;
static ui64   internal_ECS_capacity            = 1024;

enum {
	INTERNAL_ECS_ENTITY_GROWTH_FACTOR = 2,
};

ui64 lite_engine_ECS_get_count(void) {
	return internal_ECS_entities_count;	
}

ui64 lite_engine_ECS_get_capacity(void) {
	return internal_ECS_capacity;	
}

void lite_engine_ECS_start(void) {
	internal_ECS_components = calloc(sizeof(int**), internal_ECS_capacity);
	for(ui64 i = 0; i < internal_ECS_capacity; i++) {
		internal_ECS_components[i] = calloc(sizeof(int*), internal_ECS_capacity);
	}
}

void lite_engine_ECS_stop(void) {
}

ui64 lite_engine_ECS_entity_create(void) {
	internal_ECS_entities_count++;
	return internal_ECS_entities_count;
}

void lite_engine_ECS_component_add (ui64 entity, ui8 component) {

}

