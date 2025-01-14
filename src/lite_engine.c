#include "lite_engine.h"
#include "platform_x11.h"
#include "lgl.h"
#include <time.h>

void lite_engine__viewport_size_callback(
		const unsigned int width,
		const unsigned int height) {
	lgl_viewport_set(width, height);
}

// initializes lite-engine. call this to rev up those fryers!
lite_engine_context_t *lite_engine_start(void) {
	debug_log("Rev up those fryers!");

	lite_engine_context_t *engine = calloc(sizeof(*engine), 1);
	engine->is_running	= 1;
	engine->time_current	= 0;
	engine->frame_current	= 0;
	engine->time_delta	= 0;
	engine->time_last	= 0;
	engine->time_FPS	= 0;
	engine->platform_data	= x_start("Game Window", 640, 480);

	x_data_t *x = (x_data_t*)engine->platform_data;

	x->viewport_size_callback = lite_engine__viewport_size_callback;

	glClearColor(0.2, 0.3, 0.4, 1.0);
	glEnable(GL_DEPTH_TEST);

	debug_log("Startup completed successfuly");

	return engine;
}

void lite_engine__time_update(lite_engine_context_t *engine) { // update time
	struct timespec spec;
	if (clock_gettime(CLOCK_MONOTONIC, &spec) != 0) {
		debug_error("failed to get time spec.");
		exit(0);
	}

	engine->time_current	= spec.tv_sec + spec.tv_nsec * 1e-9;
	engine->time_delta	= engine->time_current - engine->time_last;
	engine->time_last	= engine->time_current;
	engine->time_FPS	= 1 / engine->time_delta;
	engine->frame_current++;

#if 1 // log time
	debug_log( "\n"
		"time_current:	%lf\n"
		"frame_current:	%lu\n"
		"time_delta:	%lf\n"
		"time_last:	%lf\n"
		"time_FPS:	%lf",
		engine->time_current,
		engine->frame_current,
		engine->time_delta,
		engine->time_last,
		engine->time_FPS);
#endif // log time
}

// shut down and free all memory associated with the lite-engine context
void lite_engine_free(lite_engine_context_t *engine) {
	debug_log("Shutting down...");
	engine->is_running = 0;
	x_stop((x_data_t*)engine->platform_data);
	free(engine);
	debug_log("Shutdown complete");
}

void lite_engine_end_frame(lite_engine_context_t *engine) {
	x_end_frame((x_data_t*)engine->platform_data);
	lite_engine__time_update(engine);
}

