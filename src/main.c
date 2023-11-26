#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "l_renderer_gl.h"

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");

	//create stuff
	l_renderer_gl_runtime runtime = l_renderer_gl_runtime_init();	
	l_renderer_gl_camera camera = l_renderer_gl_camera_create(85.0f);
	GLuint shader = l_renderer_gl_shader_create();
	l_renderer_gl_mesh mesh = l_renderer_gl_mesh_createCube();
	l_renderer_gl_transform transform = l_renderer_gl_transform_create();
	blib_mat4_t modelMatrix = l_renderer_gl_transform_GetMatrix(&transform);
	GLuint texture = l_renderer_gl_texture_create("res/textures/test2.png");

	//texture setup
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	while (!glfwWindowShouldClose(runtime.window)){
		//early update
		{
			l_renderer_gl_runtime_update(&runtime);
			l_renderer_gl_shader_useCamera(shader, &camera);
			l_renderer_gl_shader_setUniforms(shader, modelMatrix);

			{ //Input
				{ //Keys
					int state = glfwGetKey(runtime.window, GLFW_KEY_W);
					if (state == GLFW_PRESS)
					{
						printf("W\n");
					}
					state = glfwGetKey(runtime.window, GLFW_KEY_S);
					if (state == GLFW_PRESS)
					{
						printf("S\n");
					}
					state = glfwGetKey(runtime.window, GLFW_KEY_D);
					if (state == GLFW_PRESS)
					{
						printf("D\n");
					}
					state = glfwGetKey(runtime.window, GLFW_KEY_A);
					if (state == GLFW_PRESS)
					{
						printf("A\n");
					}
				}

				{ //Mouse

				}
			}
		}

		//update
		{
			glUseProgram(shader);
			l_renderer_gl_transform_rotate(&transform,blib_vec3f_scale(
						BLIB_VEC3F_ONE,runtime.deltaTime * 2.0f));
			modelMatrix = l_renderer_gl_transform_GetMatrix(&transform);
			l_renderer_gl_camera_update(&camera, &runtime);
			l_renderer_gl_mesh_render(&mesh);
			glUseProgram(0);
		}

		//late update
		{
			glfwSwapBuffers(runtime.window);
			glfwPollEvents();
			runtime.frameEndTime = glfwGetTime();
			runtime.deltaTime = runtime.frameEndTime - runtime.frameStartTime;
			// printf("frameend: %f framestart %f deltatime: %f\n",
			// 		runtime.frameEndTime, runtime.frameStartTime, runtime.deltaTime);

			// blib_mat4_printf(modelMatrix, "model");
			// blib_mat4_printf(camera.viewMatrix, "view");
			// blib_mat4_printf(camera.projectionMatrix, "proj");
		}
	}

	l_renderer_gl_runtime_cleanup(&runtime);
	return 0;
}
