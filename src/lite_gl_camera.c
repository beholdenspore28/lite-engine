#include "lite_gl_camera.h"

blib_mat4_t lite_gl_camera_GetViewMatrix(lite_gl_transform_t* t){
	/*translation*/
	blib_mat4_t translationMat = blib_mat4_translateVec3(t->position);

	/*rotation*/
	blib_mat4_t p = blib_mat4_rotate(t->eulerAngles.x, BLIB_VEC3F_RIGHT);
	blib_mat4_t y = blib_mat4_rotate(t->eulerAngles.y, BLIB_VEC3F_UP);
	blib_mat4_t r = blib_mat4_rotate(t->eulerAngles.z, BLIB_VEC3F_FORWARD);
	blib_mat4_t rotationMat = blib_mat4_multiply(blib_mat4_multiply(r, y), p); 

	/*scale*/
	blib_mat4_t scaleMat = blib_mat4_scale(t->scale);

	/*TRS = model matrix*/
	blib_mat4_t modelMat = blib_mat4_multiply(translationMat, rotationMat);
	modelMat = blib_mat4_multiply(scaleMat, modelMat);

	return modelMat;
}

void lite_gl_camera_setProjectionMatrix(lite_gl_camera_t* cam, float aspect) {
	cam->projectionMatrix = blib_mat4_perspective(
			blib_mathf_deg2rad(cam->fov),
			aspect,
			0.01f,    /*near clip*/
			1000.0f); /*far clip*/
}

lite_gl_camera_t lite_gl_camera_create(float fov) {
	lite_gl_camera_t cam;
	
	cam.transform = lite_gl_transform_create();
	cam.transform.position.z = 5.0f;
	cam.fov = fov;

	return cam;
}

//TODO use events to tell the camera to update its projection matrix when
//the screen resolution changes
void lite_gl_camera_update(lite_gl_camera_t* cam, 
		lite_engine_instance_t* instance) {
	cam->viewMatrix = lite_gl_camera_GetViewMatrix(&cam->transform);
	lite_gl_camera_setProjectionMatrix(
			cam, (float)instance->screenWidth / (float)instance->screenHeight);
}
