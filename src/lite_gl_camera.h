#ifndef LITE_GL_CAMERA_H
#define LITE_GL_CAMERA_H

#include "blib_math.h"
#include "lite_gl_transform.h"
#include "lite.h"

typedef struct lite_gl_camera_t lite_gl_camera_t;
struct lite_gl_camera_t {
	lite_gl_transform_t transform;
	blib_vec3f_t U;
	blib_vec3f_t V;
	blib_vec3f_t N;
	blib_mat4_t projectionMatrix;
	blib_mat4_t viewMatrix;
	float fov;
};

blib_mat4_t lite_gl_camera_GetViewMatrix(lite_gl_transform_t* t);
void lite_gl_camera_setProjectionMatrix(lite_gl_camera_t* cam, float aspect);
lite_gl_camera_t lite_gl_camera_create(float fov);
void lite_gl_camera_update(lite_gl_camera_t* cam, lite_engine_instance_t* instance);
#endif /*LITE_GL_CAMERA_H*/
