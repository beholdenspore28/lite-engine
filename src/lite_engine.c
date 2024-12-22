#include "lite_engine.h"

static lite_engine_context_t *internal_context = NULL;

// initializes lite-engine. call this to rev up those fryers!
void lite_engine_start(void) {
	debug_log("Rev up those fryers!");
	debug_log("Starting...");

	internal_context = calloc(sizeof(internal_context), 1);

	switch(internal_context->renderer) {
		case LITE_ENGINE_RENDERER_GL: {
			//lite_engine_start_gl();
		}
		case LITE_ENGINE_RENDERER_NONE: {
			debug_warn("no renderer set");
		}
	}
	internal_context->is_running = 1;
	debug_log("Startup completed successfuly");
}

ui8 lite_engine_is_running(void) {
	return internal_context->is_running;
}

// returns a copy of lite-engine's internal state.
// modifying this will not change lite-engine's actual state.
// pass the modified context to lite_engine_set_context(context) to do so.
lite_engine_context_t lite_engine_context(void) {
	return *internal_context;
}

// sets the internal context pointer to one of your choosing.
//
// Modifying lite-engine's internal context while the
// engine is running is usually ill advised. remove this
// message if you don't care. you have been warned.
void lite_engine_set_context(lite_engine_context_t *context) {
	if (internal_context != NULL) {
		debug_warn(
				"Modifying lite-engine's internal context while the "
				"engine is running is usually ill advised. remove this "
				"message if you don't care. you have been warned.");

		free(internal_context);
	}
	*internal_context = *context;
}

// update the internal lite-engine state
void lite_engine_update(void) {
	debug_log("running");
}

// shut down and free all memory associated with the lite-engine context
void lite_engine_stop(void) {
	debug_log("Shutting down...");
	internal_context->is_running = 0;
	debug_log("Shutdown complete");
}
