#include "lite_engine.h"
#include "lite_engine_gl.h"

#define GLFW_INCLUDE_NONE
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "blib/blib.h"
#include "blib/blib_math3d.h"
#include "blib/blib_file.h"

DEFINE_LIST(GLuint)
DEFINE_LIST(mesh_t)
DEFINE_LIST(vertex_t)

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

static char *internal_prefer_window_title         = "Game Window";
static ui16  internal_prefer_window_size_x        = 640;
static ui16  internal_prefer_window_size_y        = 480;
static ui16  internal_prefer_window_position_x    = 0;
static ui16  internal_prefer_window_position_y    = 0;
static ui8   internal_prefer_window_always_on_top = 0;
static ui8   internal_prefer_window_fullscreen    = 0;

// object lists to keep stuff hot on the cache
ui64                  internal_gl_num_entities;
int                   internal_gl_active_camera = 0;

static object_pool_t  internal_object_pool;

void lite_engine_gl_set_prefer_window_title(char *title) {
	internal_prefer_window_title = title;
}

void lite_engine_gl_set_prefer_window_size(ui16 size_x, ui16 size_y) {
	internal_prefer_window_size_x = size_x;
	internal_prefer_window_size_y = size_y;
}

void lite_engine_gl_set_prefer_window_position(ui16 pos_x, ui16 pos_y) {
	internal_prefer_window_position_x = pos_x;
	internal_prefer_window_position_y = pos_y;
}

void lite_engine_gl_set_prefer_window_always_on_top(ui8 always_on_top) {
	internal_prefer_window_always_on_top = always_on_top;
}

void lite_engine_gl_set_prefer_window_fullscreen(ui8 fullscreen) {
	internal_prefer_window_fullscreen = fullscreen;
}

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
	
	glfwDefaultWindowHints();

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
			
	//glfwSetInputMode(internal_gl_context->window, 
	//		GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	// allocate initial pool of objects
	internal_object_pool = (object_pool_t) {
		.materials  = calloc(sizeof(*internal_object_pool.materials),  1024),
		.meshes     = calloc(sizeof(*internal_object_pool.meshes),     1024),
		.transforms = calloc(sizeof(*internal_object_pool.transforms), 1024),
		.lights     = calloc(sizeof(*internal_object_pool.lights),     1024),
		.cameras    = calloc(sizeof(*internal_object_pool.cameras),    1024),
	};

	// create a light for test scene
	const int light = 1;

	internal_object_pool.transforms[light] = (transform_t) {
		.position = { 0.0, 10, -10 },
		.rotation = quaternion_identity(),
		.scale    = vector3_one(1.0),
	};

	internal_object_pool.lights[light] = (point_light_t) {
		.diffuse = vector3_one(0.8f),
		.specular = vector3_one(1.0f),
		.constant = 1.0f,
		.linear = 0.09f,
		.quadratic = 0.0032f,
	};
		
	const int space_ship = 0;

	internal_object_pool.cameras[space_ship] = (camera_t) {
		.transform.position    = (vector3_t){ 0.0, 0.0, -30.0 },
		.transform.rotation    = quaternion_identity(),
		.transform.scale       = vector3_one(1.0),
		.projection            = matrix4_identity(),
	};

	internal_object_pool.materials[space_ship] = (material_t) {
		.shader = lite_engine_gl_shader_create(
				"res/shaders/phong_diffuse_vertex.glsl",
				"res/shaders/phong_diffuse_fragment.glsl"),
		.diffuseMap = lite_engine_gl_texture_create("res/textures/test.png"),
	};

	internal_object_pool.meshes[space_ship] = lite_engine_gl_mesh_lmod_alloc("res/models/cube.lmod");

	internal_object_pool.transforms[space_ship] = (transform_t) {
		.position = vector3_zero(),
		.rotation = quaternion_identity(),
		.scale    = vector3_one(1.0),
	};

	debug_log("OpenGL renderer initialized successfuly");
}

void lite_engine_gl_render(void) {
	//debug_log("rendering...");
#if 1 // debugging input to exit
	if (glfwGetKey(internal_gl_context->window, GLFW_KEY_ESCAPE))
		lite_engine_stop();
#endif

	{ // projection
		int window_size_x;
		int window_size_y;
		glfwGetWindowSize(
				internal_gl_context->window, 
				&window_size_x,
				&window_size_y);

		internal_gl_context->window_size_x = window_size_x;
		internal_gl_context->window_size_y = window_size_y;

		float aspect = (float)internal_gl_context->window_size_x /
			(float)internal_gl_context->window_size_y;
		internal_object_pool.cameras[internal_gl_active_camera].projection =
			matrix4_perspective(deg2rad(60), aspect, 0.0001f, 1000.0f);
		internal_object_pool.cameras[internal_gl_active_camera].transform.matrix = 
			matrix4_identity();
		lite_engine_gl_transform_calculate_view_matrix(
				&internal_object_pool.cameras[internal_gl_active_camera].transform);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	internal_object_pool.transforms->rotation = quaternion_multiply(
			internal_object_pool.transforms->rotation,
			quaternion_from_euler(vector3_one(lite_engine_gl_get_time_delta())));
 
	lite_engine_gl_mesh_update(internal_object_pool);

	glfwSwapBuffers(internal_gl_context->window);
	glfwPollEvents();
}

void lite_engine_gl_set_active_camera(int camera) {
	internal_gl_active_camera = camera;
}

void lite_engine_gl_stop(void) {
}
