#ifndef L_INPUT_H
#define L_INPUT_H

#include "blib_math.h"

typedef struct l_inputData l_inputData;
struct l_inputData {
	//mouse
	blib_vec2f_t mousePosition;
	blib_vec2f_t lastMousePosition;
	blib_vec2f_t mouseDelta;

	//keys
	blib_vec3f_t moveInputDirection;
};

#endif //L_INPUT_H
