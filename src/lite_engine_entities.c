#include "lite_engine.h"
#include <stdlib.h>
#include <assert.h>

static int* entities;
static int entity_count = 0;
static int** components;

void ecs_alloc(void) {
	entities = calloc(sizeof(entities), ENTITY_COUNT_MAX);
	components = calloc(sizeof(int**), ENTITY_COUNT_MAX);
	for(int i = 0; i < ENTITY_COUNT_MAX; i++) {
		components[i] = calloc(sizeof(int*), COMPONENT_COUNT_MAX);
	}
}

int ecs_entity_create(void) {
	++entity_count;
	assert(entity_count != ENTITY_NULL);
	assert(entity_count <= ENTITY_COUNT_MAX);
	return entity_count;
}

int ecs_component_add(int entity, int component) {
	assert(components[entity][component] != 1); // already added
	components[entity][component] = 1;
	return component;
}

void ecs_component_remove(int entity, int component) {
	assert(components[entity][component] != 0); // already removed
	components[entity][component] = 0;
}

int ecs_component_exists(int entity, int component) {
	return components[entity][component];
}
