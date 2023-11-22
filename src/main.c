#include "stdio.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

static void _l_glfw_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void _l_glfw_errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");
	
	if (!glfwInit()) {
		fprintf(stderr, "failed to init GLFW");
	}

	glfwSetErrorCallback(_l_glfw_errorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Game Window", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "Window or OpenGL context creation failed");
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, _l_glfw_keyCallback);
	
	gladLoadGL();
	//for vsync
	glfwSwapInterval(1);

	// double time = glfwGetTime();
	int width, height;
	// float aspectRatio;
	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &width, &height);
		// aspectRatio = width / (float) height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
