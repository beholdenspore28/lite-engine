#include "lite_engine.h"
#include "lite_engine_gl.h"

enum { // define component flags
	LITE_ENGINE_COMPONENT_TRANSFORM  = 1,
	LITE_ENGINE_COMPONENT_MESH       = 2,
	LITE_ENGINE_COMPONENT_LIGHT      = 2 << 1,
	LITE_ENGINE_COMPONENT_MATERIAL   = 2 << 2,
	LITE_ENGINE_COMPONENT_CAMERA     = 2 << 3,
};

// define component data

int main() {
	lite_engine_start();

	lite_engine_gl_state_t state = (lite_engine_gl_state_t) {
		.transforms   = calloc(sizeof(transform_t),   LITE_ENGINE_ENTITIES_MAX),
		.meshes       = calloc(sizeof(mesh_t),        LITE_ENGINE_ENTITIES_MAX),
		.point_lights = calloc(sizeof(point_light_t), LITE_ENGINE_ENTITIES_MAX),
		.materials    = calloc(sizeof(material_t),    LITE_ENGINE_ENTITIES_MAX),
		.cameras      = calloc(sizeof(camera_t),      LITE_ENGINE_ENTITIES_MAX),
	};
	lite_engine_gl_set_state(state);

	uint64_t cube  = lite_engine_entity_create();

	lite_engine_component_add(cube,
			LITE_ENGINE_COMPONENT_TRANSFORM |
			LITE_ENGINE_COMPONENT_MESH |
			LITE_ENGINE_COMPONENT_MATERIAL);

	while (lite_engine_is_running()) {
		for(uint64_t ID = 0; ID < LITE_ENGINE_ENTITIES_MAX; ID++) {
			//printf("ID %lu exists\n", ID);
			if (lite_engine_entity_has_component(ID, LITE_ENGINE_COMPONENT_TRANSFORM)) {
				state.transforms[ID].position.x++;
				state.transforms[ID].position.y++;
				//printf("ID %lu position { %f, %f }\n", ID, transforms[ID].position.x, transforms[ID].position.y);
			}
		}
		lite_engine_update();
	}

	return 0;
}
