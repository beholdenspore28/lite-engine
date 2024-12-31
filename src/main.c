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
		.transforms   = calloc(sizeof(transform_t), LITE_ENGINE_ENTITIES_MAX),
		.meshes       = calloc(sizeof(mesh_t),      LITE_ENGINE_ENTITIES_MAX),
		.lights       = calloc(sizeof(light_t),     LITE_ENGINE_ENTITIES_MAX),
		.materials    = calloc(sizeof(material_t),  LITE_ENGINE_ENTITIES_MAX),
		.cameras      = calloc(sizeof(camera_t),    LITE_ENGINE_ENTITIES_MAX),
	};
	lite_engine_gl_set_state(state);

	const ui64 light = lite_engine_entity_create();
	state.transforms[light] = (transform_t) {
		.position = { 0.0, 10, -10 },
		.rotation = quaternion_identity(),
		.scale    = vector3_one(1.0),
	};

	state.lights[light] = (light_t) {
		.diffuse   = vector3_one(0.8f),
		.specular  = vector3_one(1.0f),
		.constant  = 1.0f,
		.linear    = 0.09f,
		.quadratic = 0.0032f,
	};

	const ui64 camera = lite_engine_entity_create();
	state.cameras[camera] = (camera_t) {
		.projection = matrix4_identity(),
	};

	state.transforms[camera] = (transform_t) {
		.position = (vector3_t){ 0.0, 0.0, -10.0 },
		.rotation = quaternion_identity(),
		.scale    = vector3_one(1.0),
		.matrix   = matrix4_identity(),
	};

	lite_engine_gl_set_active_camera(camera);


	const ui64 cube = lite_engine_entity_create();
	state.materials[cube] = (material_t) {
		.shader = lite_engine_gl_shader_create(
				"res/shaders/phong_diffuse_vertex.glsl",
				"res/shaders/phong_diffuse_fragment.glsl"),
		.diffuseMap = lite_engine_gl_texture_create("res/textures/test.png"),
	};

	state.meshes[cube] = lite_engine_gl_mesh_lmod_alloc("res/models/untitled.lmod");
	state.meshes[cube].enabled = 1;

	state.transforms[cube] = (transform_t) {
		.position = vector3_zero(),
		.rotation = quaternion_identity(),
		.scale    = vector3_one(1.0),
	};

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
