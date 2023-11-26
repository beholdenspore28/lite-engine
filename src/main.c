#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "l_renderer_gl.h"

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");
	l_runtime_data runtime = l_runtime_init();	

	lite_gl_camera_t camera = lite_gl_camera_create(85.0f);
	GLuint shader = l_shader_create();

	l_mesh mesh = l_mesh_createCube();

	lite_gl_transform_t transform = lite_gl_transform_create();

	blib_mat4_t modelMatrix = lite_gl_transform_GetMatrix(&transform);

	GLuint texture = lite_gl_texture_create("res/textures/test2.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	while (!glfwWindowShouldClose(runtime.window)){
		//early update
		{
			//runtime update
			{
				runtime.frameStartTime = glfwGetTime();

				glfwGetFramebufferSize(
						runtime.window, 
						&runtime.windowWidth, 
						&runtime.windowHeight
						);
				runtime.aspectRatio = 
					(float)runtime.windowWidth / (float)runtime.windowHeight;
				glViewport(0, 0, runtime.windowWidth, runtime.windowHeight);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			//gameobject update
			{
				glUseProgram(shader);
				glUniform1i(glGetUniformLocation(shader, "i_texCoord"), 0);
				GLuint modelMatrixUniformLocation = glGetUniformLocation(
						shader, 
						"u_modelMatrix"
						);
				if (modelMatrixUniformLocation >= 0) {
					glUniformMatrix4fv(
							modelMatrixUniformLocation,
							1,
							GL_FALSE,
							&modelMatrix.elements[0]
							);
				}
				else {
					fprintf(
							stderr,
							"failed to locate model matrix uniform. %s %i", 
							__FILE__, 
							__LINE__
							);
				}
			}

			//camera update
			{

				GLuint viewMatrixUniformLocation = glGetUniformLocation(
						shader, 
						"u_viewMatrix"
						);
				if (viewMatrixUniformLocation >= 0) {
					glUniformMatrix4fv(
							viewMatrixUniformLocation,
							1,
							GL_FALSE,
							&camera.viewMatrix.elements[0]
							);
				}
				else {
					fprintf(
							stderr,
							"failed to locate view matrix uniform. %s %i", 
							__FILE__, 
							__LINE__
							);
				}

				GLuint projectionMatrixUniformLocation = glGetUniformLocation(
						shader, 
						"u_projectionMatrix"
						);
				if (projectionMatrixUniformLocation >= 0) {
					glUniformMatrix4fv(
							projectionMatrixUniformLocation,
							1,
							GL_FALSE,
							&camera.projectionMatrix.elements[0]
							);
				}
				else {
					fprintf(
							stderr,
							"failed to locate projection matrix uniform. %s %i",
							__FILE__, 
							__LINE__
							);
				}
			}
		}

		//update
		{
			lite_gl_transform_rotate(&transform,blib_vec3f_scale(
						BLIB_VEC3F_ONE,runtime.deltaTime * 2.0f));
			modelMatrix = lite_gl_transform_GetMatrix(&transform);
			lite_gl_camera_update(&camera, &runtime);
			l_mesh_render(&mesh);
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

	l_runtime_cleanup(&runtime);
	return 0;
}
