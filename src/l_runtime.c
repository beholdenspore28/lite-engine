#include <stdio.h>
#include "stdlib.h"
#include "l_renderer_gl.h"

#define SCR_WIDTH 640
#define SCR_HEIGHT 480

static void _l_renderer_gl_runtime_keyCallback(
		GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void _l_renderer_gl_runtime_errorCallback(int error, const char* description){
	fprintf(stderr, "Error: %s\n", description);
}

static void APIENTRY _l_renderer_gl_runtime_debugMessageCallback(
		GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar *msg, const void *data)
{
	char* _source;
	char* _type;
	char* _severity;

	switch (source) {
		case GL_DEBUG_SOURCE_API:
			_source = "API";
			break;

		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			_source = "WINDOW SYSTEM";
			break;

		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			_source = "SHADER COMPILER";
			break;

		case GL_DEBUG_SOURCE_THIRD_PARTY:
			_source = "THIRD PARTY";
			break;

		case GL_DEBUG_SOURCE_APPLICATION:
			_source = "APPLICATION";
			break;

		case GL_DEBUG_SOURCE_OTHER:
			_source = "UNKNOWN";
			break;

		default:
			_source = "UNKNOWN";
			break;
	}

	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			_type = "ERROR";
			break;

		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			_type = "DEPRECATED BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			_type = "UDEFINED BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_PORTABILITY:
			_type = "PORTABILITY";
			break;

		case GL_DEBUG_TYPE_PERFORMANCE:
			_type = "PERFORMANCE";
			break;

		case GL_DEBUG_TYPE_OTHER:
			_type = "OTHER";
			break;

		case GL_DEBUG_TYPE_MARKER:
			_type = "MARKER";
			break;

		default:
			_type = "UNKNOWN";
			break;
	}

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			_severity = "HIGH";
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			_severity = "MEDIUM";
			break;

		case GL_DEBUG_SEVERITY_LOW:
			_severity = "LOW";
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			_severity = "NOTIFICATION";
			break;

		default:
			_severity = "UNKNOWN";
			break;
	}
	printf("%d: [%s] [%s SEVERITY] raised from %s: %s\n\n",
			id, _type, _severity, _source, msg);
}

l_renderer_gl_runtime l_renderer_gl_runtime_init(void){
	if (!glfwInit()) {
		fprintf(stderr, "failed to init GLFW");
	}

	glfwSetErrorCallback(_l_renderer_gl_runtime_errorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(
			SCR_WIDTH,SCR_HEIGHT,"Game Window",NULL,NULL);
	if (!window){
		fprintf(stderr, "Window or OpenGL context creation failed");
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, _l_renderer_gl_runtime_keyCallback);

	if (!gladLoadGL()) {
		fprintf(stderr,"failed to load GLAD!");
		exit(1);
	}
	//for vsync
	// glfwSwapInterval(1);
	glEnable(GL_DEBUG_OUTPUT);
	// glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&_l_renderer_gl_runtime_debugMessageCallback,NULL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);

	l_renderer_gl_runtime data = {};
	data.window = window;

	printf("==========================================================\n");
	printf("OPENGL INFO\n");
	printf("Vendor\t%s\n", glGetString(GL_VENDOR));
	printf("Renderer\t%s\n", glGetString(GL_RENDERER));
	printf("Version\t%s\n", glGetString(GL_VERSION));
	printf("Shading Language\t%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("==========================================================\n");

	return data;
}

void l_renderer_gl_runtime_cleanup(l_renderer_gl_runtime* d){
	glfwDestroyWindow(d->window);
	glfwTerminate();
}
