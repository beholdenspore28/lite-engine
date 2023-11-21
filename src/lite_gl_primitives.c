#include "lite_gl_primitives.h"
#include "lite.h"
#include "lite_gl_shader.h"
#include "lite_gl_texture.h"

#define _LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH 64
#define _LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH 36

static GLfloat _LITE_PRIMITIVE_CUBE_VERTEX_DATA[_LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH] = {
	/*front*/
	/*position        //color           //texcoord*/
	-0.5f,-0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 1.0f, 0.0f,/*bottom left*/
	0.5f,-0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 0.0f, 0.0f,/*bottom right*/
	-0.5f, 0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 1.0f, 1.0f,/*top left*/
	0.5f, 0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 0.0f, 1.0f,/*top right*/

	/*back*/
	-0.5f,-0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f,/*bottom left*/
	0.5f,-0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,/*bottom right*/
	-0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f,/*top left*/
	0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f,/*top right*/
};

static GLuint _LITE_PRIMITIVE_CUBE_INDEX_DATA[_LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH] = {
	/*front*/
	2,0,1, 3,2,1,
	/*right*/
	1,5,7, 7,3,1,
	/*back*/
	5,4,6, 5,6,7,
	/*left*/
	0,2,6, 0,6,4,
	/*top*/
	3,7,6, 2,3,6,
	/*bottom*/
	5,1,0, 0,4,5,
};

lite_gl_cube_t lite_gl_cube_create(){
	lite_gl_cube_t go;
	go.shader = lite_gl_pipeline_create();
	go.mesh = lite_gl_mesh_create(
			_LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH, 
			_LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH, 
			_LITE_PRIMITIVE_CUBE_INDEX_DATA,
			_LITE_PRIMITIVE_CUBE_VERTEX_DATA);
	go.texture = lite_gl_texture_create(
			"res/textures/test2.png");
	go.active = true;
	go.transform = lite_gl_transform_create();

	/*TODO texture glitch here*/
	/*DO NOT move this to update loop*/
	glUseProgram(go.shader);
	glUniform1i(glGetUniformLocation(go.shader, "texture"), 0);
	glUseProgram(0);

	return go;
}

void lite_gl_cube_update(
		lite_gl_cube_t* go, lite_engine_instance_t* instance){
	if (go->active == false) return;
	glUseProgram(go->shader);

	/*model matrix*/
	blib_mat4_t modelMat = lite_gl_transform_GetMatrix(
			&go->transform);

	GLint modelMatrixLocation = glGetUniformLocation(
			go->shader, "u_modelMatrix");

	if (modelMatrixLocation >= 0) {
		glUniformMatrix4fv(
				modelMatrixLocation,
				1,
				GL_FALSE,
				&modelMat.elements[0]);
	} else {
		lite_printError("failed to locate model matrix uniform", 
				__FILE__, __LINE__);
	}

	/*
	TODO cache projectionMatrixLocation in ram to prevent
	calling GetUniformLocation every frame
	*/

	/*projection matrix*/
	GLint projectionMatrixLocation = glGetUniformLocation(
			go->shader, "u_projectionMatrix");

	if (projectionMatrixLocation >= 0) {
		glUniformMatrix4fv(
				projectionMatrixLocation,
				1,
				GL_FALSE,
				&TESTcamera.projectionMatrix.elements[0]);
	} else {
		lite_printError("failed to locate projection matrix uniform", 
				__FILE__, __LINE__);
	}

	GLint viewMatrixLocation = glGetUniformLocation(
			go->shader, "u_viewMatrix");

	if (viewMatrixLocation >= 0) {
		glUniformMatrix4fv(
				viewMatrixLocation,
				1,
				GL_FALSE,
				&TESTcamera.viewMatrix.elements[0]);
	} else {
		lite_printError("failed to locate view matrix uniform", 
				__FILE__, __LINE__);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, go->texture);

	lite_gl_mesh_render(&go->mesh);
}
