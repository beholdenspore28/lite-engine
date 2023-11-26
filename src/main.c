#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "l_renderer_gl.h"

int getkey(l_renderer_gl_runtime* r, int key) {
	int state = glfwGetKey(r->window,key);
	return state == GLFW_PRESS;
}

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

	blib_vec2f_t mousePosition = BLIB_VEC2F_ZERO;
	blib_vec2f_t lastMousePosition = BLIB_VEC2F_ZERO;
	blib_vec2f_t mouseDelta = BLIB_VEC2F_ZERO;
	blib_vec3f_t moveDirection = BLIB_VEC3F_ZERO;

	while (!glfwWindowShouldClose(runtime.window)){
		//early update
		{
			l_renderer_gl_runtime_update(&runtime);
			l_renderer_gl_shader_useCamera(shader, &camera);
			l_renderer_gl_shader_setUniforms(shader, modelMatrix);

			{ //Input
				{ //Keys
					moveDirection.z = getkey(&runtime, GLFW_KEY_W) - getkey(&runtime, GLFW_KEY_S);
					moveDirection.x = getkey(&runtime, GLFW_KEY_D) - getkey(&runtime, GLFW_KEY_A);
					moveDirection = blib_vec3f_normalize(moveDirection);
					printf("moveDirection [%f, %f, %f]\n", moveDirection.x, moveDirection.y, moveDirection.z);
				}

				{ //Mouse
					mouseDelta = blib_vec2f_subtract(lastMousePosition, mousePosition);
					lastMousePosition = mousePosition;

					double xpos, ypos;
					glfwGetCursorPos(runtime.window, &xpos, &ypos);
					mousePosition.x = (float) xpos - runtime.windowWidth * 0.5f;
					mousePosition.y = (float) ypos - runtime.windowHeight * 0.5f;

					printf("mouse delta [%f, %f]\n", mouseDelta.x, mouseDelta.y);
					printf("mouse pos [%f, %f]\n", mousePosition.x, mousePosition.y);
				}
			}
		}

		//update
		{
			glUseProgram(shader);
			// l_renderer_gl_transform_rotate(&transform,blib_vec3f_scale(
			// 			BLIB_VEC3F_ONE,runtime.deltaTime * 2.0f));
			modelMatrix = l_renderer_gl_transform_GetMatrix(&transform);
			
			{ //camera mouse look
				float camRotSpeed = runtime.deltaTime * 0.25f;
				camera.transform.eulerAngles.y += mouseDelta.x * camRotSpeed;
				camera.transform.eulerAngles.x = blib_mathf_clamp(
						camera.transform.eulerAngles.x + mouseDelta.y * camRotSpeed, 
						-BLIB_PI/2, 
						BLIB_PI/2
						);
			}

			{	//move camera
				camera.transform.position = blib_vec3f_add(
						camera.transform.position, 
						blib_vec3f_scale(moveDirection, runtime.deltaTime)
						);
			}

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
