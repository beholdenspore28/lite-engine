#ifndef L_TRANSFORM_H
#define L_TRANSFORM_H

#include "blib_math.h"

typedef struct {
	blib_vec3f_t position;
	blib_vec3f_t eulerAngles;
	blib_vec3f_t scale;

} lite_gl_transform_t;

lite_gl_transform_t lite_gl_transform_create();
blib_mat4_t lite_gl_transform_GetMatrix(lite_gl_transform_t* t);
blib_vec3f_t lite_transform_getLocalForward(lite_gl_transform_t* t);
blib_vec3f_t lite_transform_getLocalUp(lite_gl_transform_t* t);
blib_vec3f_t lite_transform_getLocalRight(lite_gl_transform_t* t);
void lite_gl_transform_rotate(lite_gl_transform_t* t, blib_vec3f_t rotation);

#endif /*L_TRANSFORM_H*/
