#ifndef L_ENGINE_H
#define L_ENGINE_H

#include "l_renderer_gl.h"

typedef struct l_engineData l_engineData;
typedef struct l_inputData l_inputData;

struct l_inputData {
	//mouse
	blib_vec2f_t mousePosition;
	blib_vec2f_t lastMousePosition;
	blib_vec2f_t mouseDelta;

	//keys
	blib_vec3f_t moveInputDirection;
};

struct l_engineData {
	l_renderer_gl rendererGL;
	l_inputData inputData;
	
	blib_mat4_t modelMatrix;
	l_renderer_gl_transform transform;
	l_renderer_gl_mesh mesh;

	blib_mat4_t modelMatrix1;
	l_renderer_gl_transform transform1;
	l_renderer_gl_mesh mesh1;
};

#endif //L_ENGINE_H
