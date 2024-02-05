#include <stdio.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "l_renderer_gl.h"
#include "l_engine.h"

int l_input_getKey(l_renderer_gl* r, int key) {
	int state = glfwGetKey(r->window,key);
	return state == GLFW_PRESS;
}

void earlyUpdate(l_engineData* pEngineData) {
	l_renderer_gl_update(&pEngineData->rendererGL);
	l_renderer_gl_shader_useCamera(
			pEngineData->rendererGL.activeShader, 
			&pEngineData->rendererGL.activeCamera
			);

	l_renderer_gl_camera_update(
			&pEngineData->rendererGL.activeCamera, 
			&pEngineData->rendererGL
			);

	pEngineData->inputData.moveInputDirection.x = 
		l_input_getKey(&pEngineData->rendererGL, GLFW_KEY_A) - 
		l_input_getKey(&pEngineData->rendererGL, GLFW_KEY_D);

	pEngineData->inputData.moveInputDirection.y = 
		l_input_getKey(&pEngineData->rendererGL, GLFW_KEY_LEFT_SHIFT) - 
		l_input_getKey(&pEngineData->rendererGL, GLFW_KEY_SPACE);

	pEngineData->inputData.moveInputDirection.z = 
		l_input_getKey(&pEngineData->rendererGL, GLFW_KEY_S) - 
		l_input_getKey(&pEngineData->rendererGL, GLFW_KEY_W);

	pEngineData->inputData.moveInputDirection = 
		blib_vec3f_normalize(pEngineData->inputData.moveInputDirection);

	// blib_vec3f_printf(moveInputDirection, "moveInputDirection");
	// glfwSetInputMode(renderer.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	pEngineData->inputData.mouseDelta = blib_vec2f_subtract(
			pEngineData->inputData.lastMousePosition, 
			pEngineData->inputData.mousePosition);
	pEngineData->inputData.lastMousePosition = pEngineData->inputData.mousePosition;

	double xpos, ypos;
	glfwGetCursorPos(pEngineData->rendererGL.window, &xpos, &ypos);
	pEngineData->inputData.mousePosition.x = 
		(float) xpos - pEngineData->rendererGL.windowWidth * 0.5f;
	pEngineData->inputData.mousePosition.y = 
		(float) ypos - pEngineData->rendererGL.windowHeight * 0.5f;

	// blib_vec2f_printf(mouseDelta, "mouseDelta");
	// blib_vec2f_printf(mousePosition, "mousePosition");
}

void update(l_engineData* pEngineData) {
	l_inputData* inputData = &pEngineData->inputData;
	l_renderer_gl* renderer = &pEngineData->rendererGL;

	{ //cube update
		glUseProgram(renderer->activeShader); //TODO THIS is where you set an object-specific shader.
		l_renderer_gl_shader_setUniform3f(renderer->activeShader, "objectColor", 1.0f, 1.0f, 1.0f);
		l_renderer_gl_shader_setUniform3f(renderer->activeShader, "lightColor", 1.0f, 1.0f, 1.0f);
		glUniform1i(glGetUniformLocation(renderer->activeShader, "i_texCoord"), 0);
		l_renderer_gl_shader_setMat4Uniform(
				renderer->activeShader,
				"u_modelMatrix",
				&pEngineData->modelMatrix
				);

		l_renderer_gl_transform_rotate(
				&pEngineData->transform,blib_vec3f_scale(
					BLIB_VEC3F_ONE,renderer->deltaTime * 2.0f
					)
				);
		pEngineData->modelMatrix = l_renderer_gl_transform_GetMatrix(
				&pEngineData->transform
				);
		l_renderer_gl_mesh_render(&pEngineData->mesh);
	}

	{ //cube 1 update
		glUseProgram(renderer->activeShader); //TODO THIS is where you set an object-specific shader.
		glUniform1i(glGetUniformLocation(renderer->activeShader, "i_texCoord"), 0);
		l_renderer_gl_shader_setMat4Uniform(
				renderer->activeShader,
				"u_modelMatrix",
				&pEngineData->modelMatrix1
				);

		// l_renderer_gl_transform_rotate(&transform1,blib_vec3f_scale(
		// 			BLIB_VEC3F_ONE,renderer.deltaTime * 2.0f));

		pEngineData->transform1.position.x = sinf(renderer->frameStartTime) * 5.0f;
		pEngineData->modelMatrix1 = l_renderer_gl_transform_GetMatrix(
				&pEngineData->transform1
				);
		l_renderer_gl_mesh_render(&pEngineData->mesh1);
	}

	{ //camera mouse look
		float camRotSpeed = renderer->deltaTime * 0.25f;
		renderer->activeCamera.transform.eulerAngles.y += 
			inputData->mouseDelta.x * camRotSpeed;
		renderer->activeCamera.transform.eulerAngles.x = blib_mathf_clamp(
				renderer->activeCamera.transform.eulerAngles.x + 
				inputData->mouseDelta.y * camRotSpeed, 
				-BLIB_PI/2, 
				BLIB_PI/2
				);
	}

	{	//move camera
		//local directions
		blib_vec3f_t cameraUp = l_renderer_gl_transform_getLocalUp(
				&renderer->activeCamera.transform
				);
		blib_vec3f_t cameraRight = l_renderer_gl_transform_getLocalRight(
				&renderer->activeCamera.transform
				);
		blib_vec3f_t cameraForward = l_renderer_gl_transform_getLocalForward(
				&renderer->activeCamera.transform
				);

		float camMoveSpeed = renderer->deltaTime * 5.0f;

		cameraUp = blib_vec3f_scale(
				cameraUp, 
				camMoveSpeed * inputData->moveInputDirection.y
				);
		cameraRight = blib_vec3f_scale(
				cameraRight, 
				camMoveSpeed * inputData->moveInputDirection.x
				);
		cameraForward = blib_vec3f_scale(
				cameraForward, 
				camMoveSpeed * inputData->moveInputDirection.z
				);

		blib_vec3f_t finalMoveDirection = blib_vec3f_add(
				cameraUp,blib_vec3f_add(cameraRight,cameraForward)
				);
		renderer->activeCamera.transform.position = blib_vec3f_add(
				renderer->activeCamera.transform.position, 
				finalMoveDirection
				); 
	}
}

void lateUpdate(l_engineData* pEngineData) {
	l_renderer_gl* renderer = &pEngineData->rendererGL;

	glfwSwapBuffers(pEngineData->rendererGL.window);
	glfwPollEvents();
	renderer->frameEndTime = glfwGetTime();
	renderer->deltaTime = 
		renderer->frameEndTime - renderer->frameStartTime;
	// printf("frameend: %f framestart %f deltatime: %f\n",
	// 		renderer->frameEndTime, renderer->frameStartTime, renderer->deltaTime);

	// blib_mat4_printf(pEngineData->modelMatrix, "model");
	// blib_mat4_printf(renderer->activeCamera.viewMatrix, "view");
	// blib_mat4_printf(renderer->activeCamera.projectionMatrix, "proj");
}

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");

	//create stuff
	l_engineData engineData = (l_engineData) {
		.inputData = (l_inputData){0},
			.rendererGL = l_renderer_gl_init(854,480),

			.modelMatrix = l_renderer_gl_transform_GetMatrix(&engineData.transform),
			.transform = l_renderer_gl_transform_create(),
			.mesh = l_renderer_gl_mesh_createCube(),

			.modelMatrix1 = l_renderer_gl_transform_GetMatrix(&engineData.transform1),
			.transform1 = l_renderer_gl_transform_create(),
			.mesh1 = l_renderer_gl_mesh_createCube()
	};
	engineData.rendererGL.activeCamera = l_renderer_gl_camera_create(85.0f);
	engineData.rendererGL.activeShader = l_renderer_gl_shader_create();

	//texture setup
	GLuint texture = l_renderer_gl_texture_create("res/textures/test.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	//main loop
	while (!glfwWindowShouldClose(engineData.rendererGL.window)){
		earlyUpdate(&engineData);
		update(&engineData);
		lateUpdate(&engineData);
	}

	l_renderer_gl_cleanup(&engineData.rendererGL);
	return 0;
}
