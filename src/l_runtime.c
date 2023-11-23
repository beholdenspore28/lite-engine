#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "l_runtime.h"
#include "stdlib.h"

#define SCR_WIDTH 640
#define SCR_HEIGHT 480

static void _l_runtime_keyCallback(
		GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void _l_runtime_errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

l_runtime_data l_runtime_init(void){
	if (!glfwInit()) {
		fprintf(stderr, "failed to init GLFW");
	}

	glfwSetErrorCallback(_l_runtime_errorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(
			SCR_WIDTH, SCR_HEIGHT, "Game Window", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "Window or OpenGL context creation failed");
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, _l_runtime_keyCallback);

	if (!gladLoadGL()) {
		fprintf(stderr,"failed to load GLAD!");
		exit(1);
	}
	//for vsync
	// glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);

	l_runtime_data data = {};
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

void l_runtime_update(l_runtime_data* d) {
	// double time = glfwGetTime();
	int width, height;
	// float aspectRatio;
	glfwGetFramebufferSize(d->window, &width, &height);
	// aspectRatio = width / (float) height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(d->window);
	glfwPollEvents();
}

void l_runtime_cleanup(l_runtime_data* d){
	glfwDestroyWindow(d->window);
	glfwTerminate();
}
