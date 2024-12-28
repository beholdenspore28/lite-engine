#include "lite_engine.h"

int main() {
	//lite_engine_use_render_api(LITE_ENGINE_RENDERER_GL);
	lite_engine_start();

#if 0 // copied this code from the end of lite_engine_gl_start()
	const ui64 space_ship = lite_engine_ECS_entity_create();

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

	const ui64 space_ship = lite_engine_ECS_entity_create();

	while (lite_engine_is_running()) {
		lite_engine_update();
		//lite_engine_stop();
	}

	return 0;
}
