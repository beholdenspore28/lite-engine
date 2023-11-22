#ifndef L_GLFW_H
#define L_GLFW_H

#include "GLFW/glfw3.h"

typedef struct l_glfw_data l_glfw_data;
struct l_glfw_data {
	GLFWwindow* window;
};

l_glfw_data l_glfw_init(void);
void l_glfw_update(l_glfw_data* d);
void l_glfw_data_destroy(l_glfw_data* d);

#endif /*L_GLFW_H*/
