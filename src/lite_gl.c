#include "stb_image.h"
#include "blib/blib.h"
#include "lite_gl.h"
#include "lite_engine_log.h"

#include <ctype.h>

DEFINE_LIST(GLint)
DEFINE_LIST(GLuint)

void key_callback(GLFWwindow *window, const int key, const int scancode,
		const int action, const int mods) {
	(void)scancode;
	(void)mods;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

