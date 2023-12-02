#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "l_renderer_gl.h"

int getkey(l_renderer_gl* r, int key) {
	int state = glfwGetKey(r->window,key);
	return state == GLFW_PRESS;
}

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");

	//create stuff
	l_renderer_gl renderer = l_renderer_gl_init();	
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
	blib_vec3f_t moveInputDirection = BLIB_VEC3F_ZERO;

	while (!glfwWindowShouldClose(renderer.window)){
		//early update
		{
			l_renderer_gl_update(&renderer);
			l_renderer_gl_shader_useCamera(shader, &camera);
			l_renderer_gl_shader_setUniforms(shader, modelMatrix);
			l_renderer_gl_camera_update(&camera, &renderer);

			{ //Input
				{ //Keys
					moveInputDirection.x = 
						getkey(&renderer, GLFW_KEY_A) - 
						getkey(&renderer, GLFW_KEY_D);
					moveInputDirection.y = 
						getkey(&renderer, GLFW_KEY_LEFT_SHIFT) - 
						getkey(&renderer, GLFW_KEY_SPACE);
					moveInputDirection.z = 
						getkey(&renderer, GLFW_KEY_S) - 
						getkey(&renderer, GLFW_KEY_W);
					moveInputDirection = blib_vec3f_normalize(moveInputDirection);
					// blib_vec3f_printf(moveInputDirection, "moveInputDirection");
				}

				{ //Mouse
					glfwSetInputMode(renderer.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					mouseDelta = blib_vec2f_subtract(lastMousePosition, mousePosition);
					lastMousePosition = mousePosition;

					double xpos, ypos;
					glfwGetCursorPos(renderer.window, &xpos, &ypos);
					mousePosition.x = (float) xpos - renderer.windowWidth * 0.5f;
					mousePosition.y = (float) ypos - renderer.windowHeight * 0.5f;

					// blib_vec2f_printf(mouseDelta, "mouseDelta");
					// blib_vec2f_printf(mousePosition, "mousePosition");
				}
			}
		}

		//update
		{
			glUseProgram(shader);
			// l_renderer_gl_transform_rotate(&transform,blib_vec3f_scale(
			// 			BLIB_VEC3F_ONE,renderer.deltaTime * 2.0f));
			modelMatrix = l_renderer_gl_transform_GetMatrix(&transform);
			
			{ //camera mouse look
				float camRotSpeed = renderer.deltaTime * 0.25f;
				camera.transform.eulerAngles.y += mouseDelta.x * camRotSpeed;
				camera.transform.eulerAngles.x = blib_mathf_clamp(
						camera.transform.eulerAngles.x + mouseDelta.y * camRotSpeed, 
						-BLIB_PI/2, 
						BLIB_PI/2
						);
			}

			{	//move camera
				//local directions
				blib_vec3f_t cameraUp = l_renderer_gl_transform_getLocalUp(
						&camera.transform
						);
				blib_vec3f_t cameraRight = l_renderer_gl_transform_getLocalRight(
						&camera.transform
						);
				blib_vec3f_t cameraForward = l_renderer_gl_transform_getLocalForward(
						&camera.transform
						);

				float camMoveSpeed = renderer.deltaTime * 5.0f;

				cameraUp = blib_vec3f_scale(
						cameraUp, 
						camMoveSpeed * moveInputDirection.y
						);
				cameraRight = blib_vec3f_scale(
						cameraRight, 
						camMoveSpeed * moveInputDirection.x
						);
				cameraForward = blib_vec3f_scale(
						cameraForward, 
						camMoveSpeed * moveInputDirection.z
						);

				blib_vec3f_t finalMoveDirection = blib_vec3f_add(
						cameraUp,blib_vec3f_add(cameraRight,cameraForward)
						);
				camera.transform.position = blib_vec3f_add(
						camera.transform.position, 
						finalMoveDirection
						); 
			}

			l_renderer_gl_mesh_render(&mesh);
			glUseProgram(0);
		}

		//late update
		{
			glfwSwapBuffers(renderer.window);
			glfwPollEvents();
			renderer.frameEndTime = glfwGetTime();
			renderer.deltaTime = renderer.frameEndTime - renderer.frameStartTime;
			// printf("frameend: %f framestart %f deltatime: %f\n",
			// 		renderer.frameEndTime, renderer.frameStartTime, renderer.deltaTime);

			// blib_mat4_printf(modelMatrix, "model");
			// blib_mat4_printf(camera.viewMatrix, "view");
			// blib_mat4_printf(camera.projectionMatrix, "proj");
		}
	}

	l_renderer_gl_cleanup(&renderer);
	return 0;
}
