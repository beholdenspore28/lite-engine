#include <stdio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "l_runtime.h"

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");
	l_runtime_data runtime = l_runtime_init();	

	while (!glfwWindowShouldClose(runtime.window)){
		l_runtime_update(&runtime);
	}

	l_runtime_cleanup(&runtime);
	return 0;
}
