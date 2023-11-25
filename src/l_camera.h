#ifndef L_CAMERA_H
#define L_CAMERA_H

#include "blib_math.h"
#include "l_transform.h"
#include "l_runtime.h"

typedef struct lite_gl_camera_t lite_gl_camera_t;
struct lite_gl_camera_t {
	lite_gl_transform_t transform;
	blib_mat4_t projectionMatrix;
	blib_mat4_t viewMatrix;
	float fov;
};

lite_gl_camera_t lite_gl_camera_create(float fov);
blib_mat4_t lite_gl_camera_GetViewMatrix(lite_gl_transform_t* t);
void lite_gl_camera_setProjectionMatrix(lite_gl_camera_t* cam, float aspect);
void lite_gl_camera_update(lite_gl_camera_t* cam,l_runtime_data* d);

#endif /*L_CAMERA_H*/
