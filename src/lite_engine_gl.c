#include "lite_engine_gl.h"
#include "lite_engine.h"

#include "glad/gl.h"
#include "platform_x11.h"

#include "blib/blib.h"
#include "blib/blib_file.h"
#include "blib/blib_math3d.h"

DEFINE_LIST(GLuint)
DEFINE_LIST(mesh_t)
DEFINE_LIST(vertex_t)

static x_data_t *internal_x_data = NULL;

typedef struct {
	char	*window_title;
	ui16	window_size_x;
	ui16	window_size_y;
	ui16	window_position_x;
	ui16	window_position_y;
	ui8	window_always_on_top;
	ui8	window_fullscreen;
} opengl_context_t;

static opengl_context_t	*lgl__context		= NULL;
static ui64		lgl__active_camera	= 0;
static lgl_state_t lgl__state		= {0};

static char	*lgl__prefer_window_title		= "Game Window";
static ui16	lgl__prefer_window_size_x		= 640;
static ui16	lgl__prefer_window_size_y		= 480;
static ui16	lgl__prefer_window_position_x	= 0;
static ui16	lgl__prefer_window_position_y	= 0;
static ui8	lgl__prefer_window_always_on_top	= 0;
static ui8	lgl__prefer_window_fullscreen	= 0;

ui64 lgl_get_active_camera(void) {
	return lgl__active_camera;
}

void lgl_set_state(lgl_state_t state) {
	lgl__state = state;
}

void lgl_set_prefer_window_title(char *title) {
	lgl__prefer_window_title = title;
}

void lgl_set_prefer_window_size(ui16 size_x, ui16 size_y) {
	lgl__prefer_window_size_x = size_x;
	lgl__prefer_window_size_y = size_y;
}

void lgl_set_prefer_window_position(ui16 pos_x, ui16 pos_y) {
	lgl__prefer_window_position_x = pos_x;
	lgl__prefer_window_position_y = pos_y;
}

void lgl_set_prefer_window_always_on_top(ui8 always_on_top) {
	lgl__prefer_window_always_on_top = always_on_top;
}

void lgl_set_prefer_window_fullscreen(ui8 fullscreen) {
	lgl__prefer_window_fullscreen = fullscreen;
}

/*
void APIENTRY glDebugOutput(
		const GLenum source, const GLenum type,
		const unsigned int id, const GLenum severity,
		const GLsizei length, const char *message,
		const void *userParam) {
	(void)length;
	(void)userParam;

	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
		return;
	}

	printf("---------------\n");
	printf("Debug message (%d) %s\n", id, message);

	switch (source) {
	case GL_DEBUG_SOURCE_API: printf("Source: API"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		printf("Source: Window System");
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		printf("Source: Shader Compiler");
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: printf("Source: Third Party"); break;
	case GL_DEBUG_SOURCE_APPLICATION: printf("Source: Application"); break;
	case GL_DEBUG_SOURCE_OTHER: printf("Source: Other"); break;
	}
	printf("\n");

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		printf("Type: Error");
	} break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		printf("Type: Deprecated Behaviour");
	} break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		printf("Type: Undefined Behaviour");
	} break;
	case GL_DEBUG_TYPE_PORTABILITY: {
		printf("Type: Portability");
	} break;
	case GL_DEBUG_TYPE_PERFORMANCE: {
		printf("Type: Performance");
	} break;
	case GL_DEBUG_TYPE_MARKER: {
		printf("Type: Marker");
	} break;
	case GL_DEBUG_TYPE_PUSH_GROUP: {
		printf("Type: Push Group");
	} break;
	case GL_DEBUG_TYPE_POP_GROUP: {
		printf("Type: Pop Group");
	} break;
	case GL_DEBUG_TYPE_OTHER: {
		printf("Type: Other");
	} break;
	}
	printf("\n");

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		printf("Severity: high");
	} break;
	case GL_DEBUG_SEVERITY_MEDIUM: {
		printf("Severity: medium");
	} break;
	case GL_DEBUG_SEVERITY_LOW: {
		printf("Severity: low");
	} break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: {
		printf("Severity: notification");
	} break;
	}
	printf("\n\n");
}
*/

void lgl_start(void) {
	debug_log("initializing OpenGL renderer.");

	lgl__context = calloc(sizeof(*lgl__context), 1);
	lgl__context->window_title		= lgl__prefer_window_title;
	lgl__context->window_size_x		= lgl__prefer_window_size_x;
	lgl__context->window_size_y		= lgl__prefer_window_size_y;
	lgl__context->window_position_x		= lgl__prefer_window_position_x;
	lgl__context->window_position_y		= lgl__prefer_window_position_y;
	lgl__context->window_always_on_top	= lgl__prefer_window_always_on_top;
	lgl__context->window_fullscreen		= lgl__prefer_window_fullscreen;

	internal_x_data = x_start("Game Window",
			lgl__context->window_size_x,
			lgl__context->window_size_y);

	//assert(lgl__context->window != NULL);

	//if (!gladLoadGL()) { debug_error("Failed to initialize GLAD"); }

	//int flags;
	//glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	//if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
	//	debug_error("Failed to set debug context flag");
	//} else {
	//	glEnable(GL_DEBUG_OUTPUT);
	//	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//	glDebugMessageCallback(glDebugOutput, NULL);
	//	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	//}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void lgl_render(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	lgl_mesh_update	(lgl__state);
	x_end_frame(internal_x_data);
}

void lgl_set_active_camera(ui64 camera) {
	lgl__active_camera = camera;
}

void lgl_stop(void) {
	x_stop(internal_x_data);
}
