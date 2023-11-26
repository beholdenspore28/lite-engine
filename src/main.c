#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "l_renderer_gl.h"

GLuint l_renderer_gl_shader_getMatrixLocation(
		GLuint shader, 
		const char* uniformName, 
		blib_mat4_t* m){
	GLuint MatrixUniformLocation = glGetUniformLocation(
			shader, 
			uniformName
			);
	if (MatrixUniformLocation >= 0) {
		glUniformMatrix4fv(
				MatrixUniformLocation,
				1,
				GL_FALSE,
				&m->elements[0]
				);
	}
	else {
		fprintf(
				stderr,
				"failed to locate matrix uniform. %s %i",
				__FILE__, 
				__LINE__
				);
	}
	return MatrixUniformLocation;
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
				l_renderer_gl_shader_getMatrixLocation(
						shader,
						"u_modelMatrix",
						&modelMatrix
						);
			}

			//camera update
			{
				l_renderer_gl_shader_getMatrixLocation(
						shader,
						"u_viewMatrix",
						&camera.viewMatrix
						);
				l_renderer_gl_shader_getMatrixLocation(
						shader, 
						"u_projectionMatrix", 
						&camera.projectionMatrix
						);
			}
		}

		//update
		{
			l_renderer_gl_transform_rotate(&transform,blib_vec3f_scale(
						BLIB_VEC3F_ONE,runtime.deltaTime * 2.0f));
			modelMatrix = l_renderer_gl_transform_GetMatrix(&transform);
			l_renderer_gl_camera_update(&camera, &runtime);
			l_renderer_gl_mesh_render(&mesh);
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
