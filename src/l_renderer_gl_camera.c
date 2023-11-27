#include "l_renderer_gl.h"

blib_mat4_t l_renderer_gl_camera_GetViewMatrix(l_renderer_gl_transform* t){
	/*translation*/
	blib_mat4_t translationMat = blib_mat4_translateVec3(t->position);

	/*rotation*/
	blib_vec3f_t forward= BLIB_VEC3F_ZERO;
	forward.x = cosf(t->eulerAngles.y) * cosf(t->eulerAngles.x);
	forward.y = sinf(t->eulerAngles.x);
	forward.z = sinf(t->eulerAngles.y) * cosf(t->eulerAngles.x);
	forward = blib_vec3f_normalize(forward);

	/*scale*/
	blib_mat4_t scaleMat = blib_mat4_scale(t->scale);

	/*TRS = model matrix*/
	blib_mat4_t modelMat = BLIB_MAT4_IDENTITY;
	modelMat = blib_mat4_multiply(translationMat, modelMat);
	modelMat = blib_mat4_multiply(scaleMat, modelMat);
	modelMat = blib_mat4_lookAt(
			t->position,
			blib_vec3f_add(t->position, forward),
			BLIB_VEC3F_UP
			);

	return modelMat;
}

void l_renderer_gl_camera_setProjectionMatrix(
		l_renderer_gl_camera* cam, float aspect) {
	cam->projectionMatrix = blib_mat4_perspective(
			blib_mathf_deg2rad(cam->fov),
			aspect,
			0.01f,    /*near clip*/
			1000.0f); /*far clip*/
}

l_renderer_gl_camera l_renderer_gl_camera_create(float fov) {
	l_renderer_gl_camera cam;
	
	cam.transform = l_renderer_gl_transform_create();
	cam.transform.position.z = 1.0f;
	cam.fov = fov;
	cam.viewMatrix = BLIB_MAT4_IDENTITY;
	cam.projectionMatrix = BLIB_MAT4_IDENTITY;

	return cam;
}

//TODO use events to tell the camera to update its projection matrix when
//the screen resolution changes
void l_renderer_gl_camera_update(
		l_renderer_gl_camera* cam,l_renderer_gl_runtime* d) {
	cam->viewMatrix = l_renderer_gl_camera_GetViewMatrix(&cam->transform);
	l_renderer_gl_camera_setProjectionMatrix(
			cam, (float)d->windowWidth / (float)d->windowHeight);
}
