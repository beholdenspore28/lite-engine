#include "lite_engine.h"
#include "platform_x11.h"
#include "lgl.h"

#define BLIB_IMPLEMENTATION
#include "blib/blib_file.h"
#include "blib/blib_math3d.h"

#include <time.h>

static lite_engine_context_t *internal_engine_context = NULL;

double lite_engine_get_time_delta(void) {
	return internal_engine_context->time_delta;
}

void viewport_size_callback(
		const unsigned int width,
		const unsigned int height) {
	lgl_viewport_set(width, height);
}

// initializes lite-engine. call this to rev up those fryers!
void lite_engine_start(void) {
	debug_log("Rev up those fryers!");

	internal_engine_context = calloc(sizeof(*internal_engine_context), 1);

	internal_engine_context->is_running	= 1;
	internal_engine_context->time_current	= 0;
	internal_engine_context->frame_current	= 0;
	internal_engine_context->time_delta	= 0;
	internal_engine_context->time_last	= 0;
	internal_engine_context->time_FPS	= 0;

	x_data_t *x = x_start("Game Window", 640, 480);
	x->viewport_size_callback = viewport_size_callback;
	glClearColor(0.2, 0.3, 0.4, 1.0);
	glEnable(GL_DEPTH_TEST);

	debug_log("Startup completed successfuly");
}

int lite_engine_is_running(void) { return internal_engine_context->is_running; }

// returns a copy of lite-engine's internal state.
// modifying this will not change lite-engine's actual state.
// pass the modified context to lite_engine_set_context(context) to do so.
lite_engine_context_t lite_engine_get_context(void) {
	return *internal_engine_context;
}

// sets the internal context pointer to one of your choosing.
//
// Modifying lite-engine's internal context while the
// engine is running is usually ill advised. remove this
// message if you don't care. you have been warned.
void lite_engine_set_context(lite_engine_context_t *context) {
	if (internal_engine_context != NULL) {
		debug_warn(
			"Modifying lite-engine's internal context while the "
			"engine is running is usually ill advised. remove this "
			"message if you don't care. you have been warned.");

		free(internal_engine_context);
	}
	*internal_engine_context = *context;
}

void internal_time_update(void) { // update time
	struct timespec spec;
	if (clock_gettime(CLOCK_MONOTONIC, &spec) != 0) {
		debug_error("failed to get time spec.");
		exit(0);
	}
	internal_engine_context->time_current = spec.tv_sec + spec.tv_nsec * 1e-9;
	internal_engine_context->time_delta = internal_engine_context->time_current - internal_engine_context->time_last;
	internal_engine_context->time_last = internal_engine_context->time_current;
	internal_engine_context->time_FPS = 1 / internal_engine_context->time_delta;
	internal_engine_context->frame_current++;

#if 0 // log time
	debug_log( "\n"
		"time_current:	%lf\n"
		"frame_current:	%lu\n"
		"time_delta:	%lf\n"
		"time_last:	%lf\n"
		"time_FPS:	%lf",
		internal_engine_context->time_current,
		internal_engine_context->frame_current,
		internal_engine_context->time_delta,
		internal_engine_context->time_last,
		internal_engine_context->time_FPS);
#endif // log time
}

// update the internal lite-engine state
void lite_engine_update(void) {
	// debug_log("running");

	internal_time_update();
}

// shut down and free all memory associated with the lite-engine context
void lite_engine_stop(void) {
	debug_log("Shutting down...");
	internal_engine_context->is_running = 0;
	debug_log("Shutdown complete");
}
