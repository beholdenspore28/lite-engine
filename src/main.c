#include "lite_engine.h"
#include "lite_engine_gl.h"

int main(const int argc, const char** argv) {
	(void)argc; (void) argv;

	lite_engine_use_render_api(LITE_ENGINE_RENDERER_GL);
	lite_engine_start();

	camera_t camera = (camera_t) {
		.transform.position    = (vector3_t){ 0.0, 0.0, -30.0 },
		.transform.rotation    = quaternion_identity(),
		.transform.scale       = vector3_one(1.0),
		.projection            = matrix4_identity(),
	};

	lite_engine_gl_set_active_camera(&camera);

	while (lite_engine_is_running()) {

		lite_engine_update();

		//lite_engine_stop();
	}

	return 0;
}
