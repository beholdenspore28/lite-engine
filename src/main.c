#include "lite_engine.h"
#include "lite_engine_gl.h"

int main(const int argc, const char** argv) {
	(void)argc; (void) argv;

	lite_engine_use_render_api(LITE_ENGINE_RENDERER_GL);
	lite_engine_start();

	GLuint unlit_shader = lite_engine_gl_shader_create(
			"res/shaders/unlit_vertex.glsl",
			"res/shaders/unlit_fragment.glsl");

	GLuint test_diffuse_map = lite_engine_gl_texture_create("res/textures/test.png");
	mesh_t space_ship       = lite_engine_gl_mesh_lmod_alloc("res/models/untitled.lmod");

	while (lite_engine_is_running()) {
		lite_engine_update();
		lite_engine_stop();
	}

	lite_engine_gl_mesh_free(&space_ship);

	return 0;
}
