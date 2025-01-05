#include "lite_engine.h"
#include "lite_engine_gl.h"

enum { // define component flags
	COMPONENT_TRANSFORM	= 1,
	COMPONENT_MESH		= 2,
	COMPONENT_LIGHT		= 2 << 1,
	COMPONENT_MATERIAL	= 2 << 2,
	COMPONENT_CAMERA	= 2 << 3,
};

int main() {
	{
		const int window_scale_divisor = 3;
		lite_engine_gl_set_prefer_window_position(
				1920, 1080);
		lite_engine_gl_set_prefer_window_size(
				1920/window_scale_divisor,
				1080/window_scale_divisor);
	}
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
		.position	= {0.0, 10, -10},
		.rotation	= quaternion_identity(),
		.scale		= vector3_one(1.0),
	};

	state.lights[light] = (light_t){
		.diffuse	= vector3_one(0.8f),
		.specular	= vector3_one(1.0f),
		.constant	= 1.0f,
		.linear		= 0.09f,
		.quadratic	= 0.0032f,
	};

	const ui64 camera = lite_engine_entity_create();
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

	ui32 shader = lite_engine_gl_shader_create(
			"res/shaders/phong_diffuse_vertex.glsl",
			"res/shaders/phong_diffuse_fragment.glsl");

	ui32 texture = lite_engine_gl_texture_create(
			"res/textures/lite-engine-icon.png");

	mesh_t mesh = lite_engine_gl_mesh_lmod_alloc("res/models/untitled.lmod");

	list_ui64 cubes = list_ui64_alloc();

	int size = 20;
	int step = 5;
	for(int i = -size; i <= size; i+= step) {
		for(int j = -size; j <= size; j+= step) {
			for(int k = -size; k <= size; k+= step) {
				ui64 cube = lite_engine_entity_create();
				list_ui64_add(&cubes, cube); 

				lite_engine_component_add(cube,
						COMPONENT_TRANSFORM |
						COMPONENT_MESH |
						COMPONENT_MATERIAL);

				state.materials[cube] = (material_t){
					.shader		= shader,
					.diffuseMap	= texture,
				};

				state.meshes[cube] = mesh; 

				state.transforms[cube] = (transform_t){
					.position	= (vector3_t) { i, j, 20 + k },
					.rotation	= quaternion_identity(),
					.scale		= vector3_one(1.0),
				};
			}
		}
	}

	while (lite_engine_is_running()) {
		lite_engine_update();

		for (uint64_t cube = 0; cube < cubes.length; cube++) {
			quaternion_t rot = quaternion_from_euler(vector3_one(
					lite_engine_get_time_delta() * 0.1));

			state.transforms[cubes.array[cube]].rotation = quaternion_multiply(
					state.transforms[cubes.array[cube]].rotation,
					rot);
		}
	}

	return 0;
}
