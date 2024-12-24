#include "lite_engine.h"
#include "lite_engine_gl.h"

#define BLIB_IMPLEMENTATION
#include "blib/blib_file.h"
#include "blib/blib_math3d.h"

typedef struct {
	ui8   renderer;
	ui8   is_running;
	ui64  time_current;
	ui64  frame_current;
	float time_delta;
	float time_last;
	float time_FPS;
} lite_engine_context_t;

static lite_engine_context_t *internal_engine_context = NULL;
static ui8                    internal_preferred_api = LITE_ENGINE_RENDERER_GL;

void lite_engine_use_render_api(ui8 api) {
	switch(api) {
		case LITE_ENGINE_RENDERER_GL: {
			internal_preferred_api = LITE_ENGINE_RENDERER_GL;	
		} break;
		case LITE_ENGINE_RENDERER_NONE: {
			internal_preferred_api = LITE_ENGINE_RENDERER_NONE;	
		} break;
		default: {
			internal_preferred_api = LITE_ENGINE_RENDERER_NONE;	
		} break;
	}
}

// initializes lite-engine. call this to rev up those fryers!
void lite_engine_start(void) {
	debug_log("Rev up those fryers!");
	debug_log("Starting...");

	internal_engine_context = calloc(sizeof(*internal_engine_context), 1);

	switch(internal_preferred_api) {
		case LITE_ENGINE_RENDERER_GL: {
			lite_engine_gl_start();
		} break;
		case LITE_ENGINE_RENDERER_NONE: {
			debug_warn("no renderer set");
		} break;
	}

	internal_engine_context->renderer      = internal_preferred_api;
	internal_engine_context->is_running    = 1;

	internal_engine_context->time_current  = 0;
	internal_engine_context->frame_current = 0;
	internal_engine_context->time_delta    = 0;
	internal_engine_context->time_last     = 0;
	internal_engine_context->time_FPS      = 0;

	debug_log("Startup completed successfuly");
}

ui8 lite_engine_is_running(void) {
	return internal_engine_context->is_running;
}

// returns a copy of lite-engine's internal state.
// modifying this will not change lite-engine's actual state.
// pass the modified context to lite_engine_set_context(context) to do so.
lite_engine_context_t lite_engine_context(void) {
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
	//internal_engine_context->time_current = glfwGetTime();
	internal_engine_context->time_delta = internal_engine_context->time_current - internal_engine_context->time_last;
	internal_engine_context->time_last  = internal_engine_context->time_current;

	internal_engine_context->time_FPS = 1 / internal_engine_context->time_delta;
	internal_engine_context->frame_current++;

#if 0 // log time
	debug_log( "\n"
		"time_current:  %lf\n"  
		"frame_current: %lu\n"  
		"time_delta:    %lf\n"   
		"time_last:     %lf\n"   
		"time_FPS:      %lf",   
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

	switch(internal_engine_context->renderer) {
		case LITE_ENGINE_RENDERER_GL: {
			lite_engine_gl_render();
		} break;
		case LITE_ENGINE_RENDERER_NONE: {
		} break;
		default: {
		} break;
	}
	internal_time_update();
}

// shut down and free all memory associated with the lite-engine context
void lite_engine_stop(void) {
	debug_log("Shutting down...");
	internal_engine_context->is_running = 0;

	switch(internal_engine_context->renderer) {
		case LITE_ENGINE_RENDERER_GL: {
			lite_engine_gl_stop();
		} break;
		case LITE_ENGINE_RENDERER_NONE: {
		} break;
		default: {
		} break;
	}

	debug_log("Shutdown complete");
}
