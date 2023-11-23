#include <stdio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "l_glfw.h"

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");
	l_glfw_data glfwData = l_glfw_init();	


	while (!glfwWindowShouldClose(glfwData.window)){
		l_glfw_update(&glfwData);
	}

	l_glfw_data_destroy(&glfwData);
	return 0;
}
