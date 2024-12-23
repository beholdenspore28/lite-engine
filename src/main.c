#include "lite_engine.h"
#include "lite_engine_gl.h"

int main(const int argc, const char** argv) {
	(void)argc; (void) argv;

	lite_engine_use_render_api(LITE_ENGINE_RENDERER_GL);
	lite_engine_start();

	while (lite_engine_is_running()) {
		lite_engine_update();
		
		//lite_engine_stop();
	}

	//lite_engine_gl_mesh_free(&space_ship);

	return 0;
}
