#ifndef L_GLFW_H
#define L_GLFW_H

#include "GLFW/glfw3.h"

typedef struct l_runtime_data l_runtime_data;
struct l_runtime_data {
	GLFWwindow* window;
};

l_runtime_data l_runtime_init(void);
void l_runtime_update(l_runtime_data* d);
void l_runtime_cleanup(l_runtime_data* d);

#endif /*L_GLFW_H*/
