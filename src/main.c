#include "lite_engine.h"
#include "lite_engine_gl.h"

int main() {
	//lite_engine_use_render_api(LITE_ENGINE_RENDERER_GL);
	lite_engine_start();
	lite_engine_gl_set_prefer_window_position(1920 / 2.0, 1080 / 2.0);

#if 0 // copied this code from the end of lite_engine_gl_start()

	const ui64 light = lite_engine_ECS_entity_create();
	const ui64 camera = lite_engine_ECS_entity_create();
	const ui64 space_ship = lite_engine_ECS_entity_create();

	internal_object_pool.transforms[light] = (transform_t) {
		.position = { 0.0, 10, -10 },
		.rotation = quaternion_identity(),
		.scale    = vector3_one(1.0),
	};

	internal_object_pool.lights[light] = (point_light_t) {
		.diffuse   = vector3_one(0.8f),
		.specular  = vector3_one(1.0f),
		.constant  = 1.0f,
		.linear    = 0.09f,
		.quadratic = 0.0032f,
	};

	internal_object_pool.cameras[camera] = (camera_t) {
		.projection = matrix4_identity(),
	};

	internal_object_pool.transforms[camera] = (transform_t) {
		.position = (vector3_t){ 0.0, 0.0, -10.0 },
		.rotation = quaternion_identity(),
		.scale    = vector3_one(1.0),
		.matrix   = matrix4_identity(),
	};

	lite_engine_gl_set_active_camera(camera);

	internal_object_pool.materials[space_ship] = (material_t) {
		.shader = lite_engine_gl_shader_create(
				"res/shaders/phong_diffuse_vertex.glsl",
				"res/shaders/phong_diffuse_fragment.glsl"),
		.diffuseMap = lite_engine_gl_texture_create("res/textures/test.png"),
	};

	internal_object_pool.meshes[space_ship] = lite_engine_gl_mesh_lmod_alloc("res/models/cube.lmod");
	internal_object_pool.meshes[space_ship].enabled = 1;

	internal_object_pool.transforms[space_ship] = (transform_t) {
		.position = vector3_zero(),
		.rotation = quaternion_identity(),
		.scale    = vector3_one(1.0),
	};

#endif

	const ui64 light = lite_engine_ECS_entity_create();
	const ui64 camera = lite_engine_ECS_entity_create();
	const ui64 space_ship = lite_engine_ECS_entity_create();

	debug_log("space_ship id: %lu", space_ship);
	debug_log("camera id: %lu",     camera);
	debug_log("light id: %lu",      light);

	while (lite_engine_is_running()) {
		lite_engine_update();
		//lite_engine_stop();
	}

	return 0;
}
