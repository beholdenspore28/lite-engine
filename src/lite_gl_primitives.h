#ifndef LITE_GL_PRIMITIVES_H
#define LITE_GL_PRIMITIVES_H

#include "lite_gl_transform.h"
#include "lite_gl_mesh.h"
#include "lite.h"
#include "glad/glad.h"

typedef struct lite_gl_cube_t lite_gl_cube_t;
struct lite_gl_cube_t {
	lite_gl_transform_t transform;
	lite_gl_mesh_t mesh;
	GLuint shader;
	GLuint texture;
	bool active;
};

lite_gl_cube_t lite_gl_cube_create();

void lite_gl_cube_update(
		lite_gl_cube_t* go, lite_engine_instance_t* instance);

#endif /*LITE_GL_PRIMITIVES_H*/
