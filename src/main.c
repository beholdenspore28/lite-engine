#include "lite_engine.h"
#include "lite_engine_gl.h"

enum { // define component flags
	LITE_ENGINE_COMPONENT_TRANSFORM = 1,
	LITE_ENGINE_COMPONENT_MESH	= 2,
	LITE_ENGINE_COMPONENT_LIGHT	= 2 << 1,
	LITE_ENGINE_COMPONENT_MATERIAL	= 2 << 2,
	LITE_ENGINE_COMPONENT_CAMERA	= 2 << 3,
};

int main() {
	lite_engine_start();

	lite_engine_gl_state_t state = (lite_engine_gl_state_t){
		.transforms	= calloc(sizeof(transform_t),	LITE_ENGINE_ENTITIES_MAX),
		.meshes		= calloc(sizeof(mesh_t),	LITE_ENGINE_ENTITIES_MAX),
		.lights		= calloc(sizeof(light_t),	LITE_ENGINE_ENTITIES_MAX),
		.materials	= calloc(sizeof(material_t),	LITE_ENGINE_ENTITIES_MAX),
		.cameras	= calloc(sizeof(camera_t),	LITE_ENGINE_ENTITIES_MAX),
	};
	lite_engine_gl_set_state(state);

	const ui64 light = lite_engine_entity_create();
	state.transforms[light]	= (transform_t){
		.position = {0.0, 10, -10},
		.rotation = quaternion_identity(),
		.scale    = vector3_one(1.0),
	};

	state.lights[light] = (light_t){
		.diffuse	= vector3_one(0.8f),
		.specular	= vector3_one(1.0f),
		.constant	= 1.0f,
		.linear		= 0.09f,
		.quadratic	= 0.0032f,
	};

	const ui64 camera= lite_engine_entity_create();
	state.cameras[camera] = (camera_t){
		.projection	= matrix4_identity(),
	};

	state.transforms[camera] = (transform_t){
		.position	= (vector3_t){0.0, 0.0, -5.0},
		.rotation	= quaternion_identity(),
		.scale		= vector3_one(1.0),
		.matrix		= matrix4_identity(),
	};

	lite_engine_gl_set_active_camera(camera);

	const ui64 cube = lite_engine_entity_create();
	lite_engine_component_add(cube,
			LITE_ENGINE_COMPONENT_TRANSFORM |
			LITE_ENGINE_COMPONENT_MESH |
			LITE_ENGINE_COMPONENT_MATERIAL);

	state.materials[cube] = (material_t){
		.shader = lite_engine_gl_shader_create(
				"res/shaders/phong_diffuse_vertex.glsl",
				"res/shaders/phong_diffuse_fragment.glsl"),
		.diffuseMap = lite_engine_gl_texture_create(
				"res/textures/lite-engine-icon.png"),
	};

	state.meshes[cube] = lite_engine_gl_mesh_lmod_alloc("res/models/untitled.lmod");

	state.transforms[cube] = (transform_t){
		.position	= vector3_zero(),
		.rotation	= quaternion_identity(),
		.scale		= vector3_one(1.0),
	};

	while (lite_engine_is_running()) {
		quaternion_t rot = quaternion_from_euler(vector3_one(lite_engine_get_time_delta() * 0.1));
		state.transforms[cube].rotation = quaternion_multiply(state.transforms[cube].rotation, rot);

		lite_engine_update();

		for (uint64_t e = 0; e < LITE_ENGINE_ENTITIES_MAX; e++) {
			if (lite_engine_entity_has_component(e, LITE_ENGINE_COMPONENT_TRANSFORM)) {

			}
		}
	}

	return 0;
}
