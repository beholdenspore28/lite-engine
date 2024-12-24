#include "lite_engine.h"
#include "lite_engine_gl.h"

#define GLFW_INCLUDE_NONE
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "blib/blib.h"

DEFINE_LIST(GLuint)

// object lists to keep stuff hot on the cache
ui64                           internal_gl_num_entities;
camera_t                      *internal_gl_active_camera = NULL;

static char *internal_prefer_window_title         = "Game Window";
static ui16  internal_prefer_window_size_x        = 640;
static ui16  internal_prefer_window_size_y        = 480;
static ui16  internal_prefer_window_position_x    = 0;
static ui16  internal_prefer_window_position_y    = 0;
static ui8   internal_prefer_window_always_on_top = 0;
static ui8   internal_prefer_window_fullscreen    = 0;


static list_mesh_t      internal_meshes;
static list_GLuint      internal_shaders;
static list_transform_t internal_transforms;

void lite_engine_gl_set_prefer_window_title(char *title) {
	internal_prefer_window_title = title;
}

void lite_engine_gl_set_prefer_window_size_x(ui16 size_x) {
	internal_prefer_window_size_x = size_x;
}

void lite_engine_gl_set_prefer_window_size_y(ui16 size_y) {
	internal_prefer_window_size_y = size_y;
}

void lite_engine_gl_set_prefer_window_position_x(ui16 pos_x) {
	internal_prefer_window_position_x = pos_x;
}

void lite_engine_gl_set_prefer_window_position_y(ui16 pos_y) {
	internal_prefer_window_position_y = pos_y;
}

void lite_engine_gl_set_prefer_window_always_on_top(ui8 always_on_top) {
	internal_prefer_window_always_on_top = always_on_top;
}

void lite_engine_gl_set_prefer_window_fullscreen(ui8 fullscreen) {
	internal_prefer_window_fullscreen = fullscreen;
}

typedef struct {
	GLFWwindow *window;
	char       *window_title;	
	ui16        window_size_x;
	ui16        window_size_y;
	ui16        window_position_x;
	ui16        window_position_y;
	ui8         window_always_on_top;
	ui8         window_fullscreen;
} opengl_context_t;

opengl_context_t *internal_gl_context = NULL;

void APIENTRY glDebugOutput(const GLenum source, const GLenum type,
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
		case GL_DEBUG_SOURCE_API:
			printf("Source: API");
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			printf("Source: Window System");
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			printf("Source: Shader Compiler");
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			printf("Source: Third Party");
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			printf("Source: Application");
			break;
		case GL_DEBUG_SOURCE_OTHER:
			printf("Source: Other");
			break;
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

void error_callback(const int error, const char *description) {
	(void)error;
	debug_error("%s", description);
}

void framebuffer_size_callback(
		GLFWwindow *window,
		const int width,
		const int height) {
	(void)window;
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, const int key, const int scancode,
		const int action, const int mods) {
	(void)scancode;
	(void)mods;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void lite_engine_gl_start(void) {
	debug_log("initializing OpenGL renderer.");

	internal_gl_context                       = calloc(sizeof(*internal_gl_context), 1);
	internal_gl_context->window_title         = internal_prefer_window_title;
	internal_gl_context->window_size_x        = internal_prefer_window_size_x;
	internal_gl_context->window_size_y        = internal_prefer_window_size_y;
	internal_gl_context->window_position_x    = internal_prefer_window_position_x; 
	internal_gl_context->window_position_y    = internal_prefer_window_position_y;
	internal_gl_context->window_always_on_top = internal_prefer_window_always_on_top; 
	internal_gl_context->window_fullscreen    = internal_prefer_window_fullscreen; 

	if (!glfwInit()) {
		debug_error("Failed to initialize GLFW");
	}

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	if (internal_gl_context->window_always_on_top) {
		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	}

	if (internal_gl_context->window_fullscreen) {
		internal_gl_context->window = glfwCreateWindow(
				internal_gl_context->window_size_x, 
				internal_gl_context->window_size_y,
				internal_gl_context->window_title, 
				glfwGetPrimaryMonitor(), NULL);
	} else {
		internal_gl_context->window = glfwCreateWindow(
				internal_gl_context->window_size_x, 
				internal_gl_context->window_size_y,
				internal_gl_context->window_title, 
				NULL, NULL);
	}

	assert(internal_gl_context->window != NULL);

	glfwSetWindowPos(internal_gl_context->window, 
			internal_gl_context->window_position_x,
			internal_gl_context->window_position_y);
	glfwShowWindow(internal_gl_context->window);
	glfwMakeContextCurrent(internal_gl_context->window);
	glfwSetKeyCallback(internal_gl_context->window, key_callback);
	glfwSetFramebufferSizeCallback(
			internal_gl_context->window, 
			framebuffer_size_callback);
	glfwSetInputMode(internal_gl_context->window, 
			GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSwapInterval(0);

	if (!gladLoadGL()) {
		debug_error("Failed to initialize GLAD");
	}

	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
		debug_error("Failed to set debug context flag");
	} else {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, 
				GL_DONT_CARE, 0, NULL, GL_TRUE);
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2, 0.3, 0.4, 1.0);

	debug_log("OpenGL renderer initialized successfuly");
}

void lite_engine_gl_render(void) {
	//debug_log("rendering...");
#if 1 // debugging input to exit
	if (glfwGetKey(internal_gl_context->window, GLFW_KEY_ESCAPE))
		lite_engine_stop();
#endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lite_engine_gl_mesh_update(
			internal_meshes,
			internal_shaders,
			internal_transforms);

	glfwSwapBuffers(internal_gl_context->window);
	glfwPollEvents();
}
