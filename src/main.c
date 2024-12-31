#include "lite_engine.h"

enum { // define component flags
	LITE_ENGINE_COMPONENT_TRANSFORM  = 1,
	LITE_ENGINE_COMPONENT_MESH       = 2,
	LITE_ENGINE_COMPONENT_LIGHT      = 2 << 1,
	LITE_ENGINE_COMPONENT_MATERIAL   = 2 << 2,
	LITE_ENGINE_COMPONENT_CAMERA     = 2 << 3,
};

// define component data
transform_t transforms  [LITE_ENGINE_ENTITIES_MAX];
mesh_t      meshes      [LITE_ENGINE_ENTITIES_MAX];

int main() {
	lite_engine_start();

	uint64_t e  = lite_engine_entity_create();
	uint64_t e1 = lite_engine_entity_create();

	lite_engine_component_add(e1,
			LITE_ENGINE_COMPONENT_TRANSFORM);

	lite_engine_component_add(e,
			LITE_ENGINE_COMPONENT_TRANSFORM);


	while (lite_engine_is_running()) {
		for(uint64_t ID = 0; ID < LITE_ENGINE_ENTITIES_MAX; ID++) {
			//printf("ID %lu exists\n", ID);
			if (lite_engine_entity_has_component(ID, LITE_ENGINE_COMPONENT_TRANSFORM)) {
				transforms[ID].position.x++;
				transforms[ID].position.y++;
				printf("ID %lu position { %f, %f }\n", ID, transforms[ID].position.x, transforms[ID].position.y);
			}
		}
		lite_engine_update();
	}

	return 0;
}
