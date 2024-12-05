#include "lite_engine.h"

static lite_engine_context_t* lite_engine_context = NULL;
static GLuint primitive_shader;
static mesh_t primitive_mesh_cube;

void lite_engine_start(void) {
	if (lite_engine_context == NULL) {
		printf("[ERROR_LITE_ENGINE] Lite-Engine context was null before calling lite_engine_start()."
				" Set it using lite_engine_set_context(context)\n");
	}
	assert(lite_engine_context != NULL);

	if (!glfwInit()) {
		printf("[ERROR_GLFW] Failed to initialize GLFW");
	}

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	if (lite_engine_context->window_always_on_top) {
		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	}
	assert(lite_engine_context->window_title != NULL);

	if (lite_engine_context->window_fullscreen) {
		lite_engine_context->window = glfwCreateWindow(
			lite_engine_context->window_size_x, 
			lite_engine_context->window_size_y,
			lite_engine_context->window_title, 
			glfwGetPrimaryMonitor(), NULL);
	} else {
		lite_engine_context->window = glfwCreateWindow(
			lite_engine_context->window_size_x, 
			lite_engine_context->window_size_y,
			lite_engine_context->window_title, NULL, NULL);
	}
	assert(lite_engine_context->window != NULL);

	glfwSetWindowPos(lite_engine_context->window, 
			lite_engine_context->window_position_x,
			lite_engine_context->window_position_y);
	glfwShowWindow(lite_engine_context->window);
	glfwMakeContextCurrent(lite_engine_context->window);
	glfwSetKeyCallback(lite_engine_context->window, key_callback);
	glfwSetFramebufferSizeCallback(
			lite_engine_context->window, 
			framebuffer_size_callback);
	glfwSetInputMode(lite_engine_context->window, 
			GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSwapInterval(0);

	if (!gladLoadGL()) {
		printf("[ERROR_GL] Failed to initialize GLAD\n");
	}

	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
		printf("[ERROR_GL] Failed to set debug context flag\n");
	} else {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
				GL_TRUE);
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	primitive_shader = shader_create(
		"res/shaders/primitive.vs.glsl",
		"res/shaders/primitive.fs.glsl");
	primitive_mesh_cube = mesh_alloc_cube();
}

void lite_engine_stop(void) {
	glfwTerminate();
}

void lite_engine_set_context(lite_engine_context_t* context) {
	lite_engine_context = context;
}

lite_engine_context_t lite_engine_get_context(void) { return *lite_engine_context; }

int lite_engine_is_running(void) {
	return !glfwWindowShouldClose(lite_engine_context->window);
}

void lite_engine_time_update(void) { // update time
	lite_engine_context->time_current = glfwGetTime();
	lite_engine_context->time_delta = lite_engine_context->time_current - lite_engine_context->time_last;
	lite_engine_context->time_last = lite_engine_context->time_current;

	lite_engine_context->time_FPS = 1 / lite_engine_context->time_delta;
	lite_engine_context->frame_current++;

#if 0 // log time
	printf("time_current   %f\n"
			"time_last     %f\n"
			"time_delta    %f\n"
			"FPS           %f\n"
			"frame_current %lu\n",
			lite_engine_get_context()->time_current, 
			lite_engine_get_context()->time_last, 
			lite_engine_get_context()->time_delta,
			lite_engine_get_context()->time_FPS, 
			lite_engine_get_context()->frame_current);
#endif // log time
}

void lite_engine_update(void) {
	glfwSwapBuffers(lite_engine_get_context().window);
	glfwPollEvents();
	lite_engine_time_update();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void primitive_draw_cube(transform_t transform, bool wireframe, vector4_t color) {
	glDisable(GL_CULL_FACE);
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(primitive_shader);

	// model matrix
	transform_calculate_matrix(&transform);

	shader_setUniformM4(primitive_shader, "u_modelMatrix", &transform.matrix);

	// view matrix
	shader_setUniformM4(primitive_shader, "u_viewMatrix",
			&lite_engine_context->active_camera->transform.matrix);

	// projection matrix
	shader_setUniformM4(primitive_shader, "u_projectionMatrix",
			&lite_engine_context->active_camera->projection);

	shader_setUniformV4(primitive_shader, "u_color", color);

	glBindVertexArray(primitive_mesh_cube.VAO);
	glDrawElements( GL_TRIANGLES, primitive_mesh_cube.
			indexCount, GL_UNSIGNED_INT, 0);
}

void lite_engine_window_set_resolution(const int x, const int y) {
	glfwSetWindowSize(lite_engine_context->window, x, y);
}

void lite_engine_window_set_position(const int x, const int y) {
	glfwSetWindowPos(lite_engine_context->window, x, y);
}

void lite_engine_set_clear_color(const float r, const float g, const float b,
		const float a) {
	glClearColor((GLfloat)r, (GLfloat)g, (GLfloat)b, (GLfloat)a);
}

