#include <stdio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "l_runtime.h"

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");
	l_runtime_data glfwData = l_runtime_init();	

	while (!glfwWindowShouldClose(glfwData.window)){
		l_runtime_update(&glfwData);
	}

	l_runtime_cleanup(&glfwData);
	return 0;
}
