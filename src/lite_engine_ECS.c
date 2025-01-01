#include "blib/blib.h"
#include "lite_engine.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
	bool	 is_enabled;
	bool	 is_destroyed;
	uint64_t component_flags;
} entity_t;

entity_t entities[LITE_ENGINE_ENTITIES_MAX] = {0};

uint64_t lite_engine_entity_create(void) {
	for (int ID = 0; ID < LITE_ENGINE_ENTITIES_MAX; ID++) {
		if (entities[ID].is_enabled == false) {
			entities[ID].is_enabled = true;
			return ID;
		}
	}

	return 0;
}

void lite_engine_component_add(uint64_t ID, uint64_t component_flag) {
	entities[ID].component_flags |= component_flag;
}

ui8 lite_engine_entity_has_component(uint64_t entity_ID,
				     uint64_t component_flag) {
	return entities[entity_ID].component_flags & component_flag;
}

void entity_destroy(entity_t e) { e.is_destroyed = true; }
