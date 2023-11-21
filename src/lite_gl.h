#ifndef LITE_GL_H
#define LITE_GL_H

#include "glad/glad.h"
#include "stb_image.h"
#include "blib_file.h"
#include "blib_math.h"
#include "lite.h"
#include "lite_gl_transform.h"
#include "lite_gl_camera.h"
#include "lite_gl_mesh.h"
#include "lite_gl_primitives.h"
#include "lite_gl_shader.h"
#include "lite_gl_texture.h"


/*TODO GET RID OF THESE!!*/
extern lite_gl_cube_t TESTcube;
extern lite_gl_camera_t TESTcamera;

void lite_gl_initialize(lite_engine_instance_t* instance);

#endif  /*LITE_GL_H*/
