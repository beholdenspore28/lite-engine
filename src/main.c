#include "lite_engine.h"

typedef struct {
	float x, y;
} position;

typedef struct {
	float x, y;
} velocity;

enum {
	LITE_ENGINE_COMPONENT_POSITION   = 1,
	LITE_ENGINE_COMPONENT_VELOCITY   = 2,
	LITE_ENGINE_COMPONENT_OTHER      = 2 << 1,
	LITE_ENGINE_COMPONENT_LUA_SCRIPT = 2 << 2,
};

position positions  [LITE_ENGINE_ENTITIES_MAX];
velocity velocities [LITE_ENGINE_ENTITIES_MAX];

int main() {
	lite_engine_start();

	uint64_t e  = lite_engine_entity_create();
	uint64_t e1 = lite_engine_entity_create();

	lite_engine_component_add(e1,
			LITE_ENGINE_COMPONENT_POSITION |
			LITE_ENGINE_COMPONENT_VELOCITY);

	lite_engine_component_add(e,
			LITE_ENGINE_COMPONENT_POSITION |
			LITE_ENGINE_COMPONENT_VELOCITY);


	while (lite_engine_is_running()) {
		for(uint64_t ID = 0; ID < LITE_ENGINE_ENTITIES_MAX; ID++) {
			//printf("ID %lu exists\n", ID);
			if (lite_engine_entity_has_component(ID, LITE_ENGINE_COMPONENT_VELOCITY)) {
				velocities[ID].x++; 
				velocities[ID].y++; 
				printf("ID %lu position { %f, %f }\n", ID, positions[ID].x, positions[ID].y);
			}
			if (lite_engine_entity_has_component(ID, LITE_ENGINE_COMPONENT_POSITION)) {
				positions[ID].x += velocities[ID].x;
				positions[ID].y += velocities[ID].y;
				printf("ID %lu position { %f, %f }\n", ID, positions[ID].x, positions[ID].y);
			}
		}
		lite_engine_update();
	}

	return 0;
}
