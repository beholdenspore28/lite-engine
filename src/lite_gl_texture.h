#ifndef LITE_GL_TEXTURE_H
#define LITE_GL_TEXTURE_H

#include "glad/glad.h"
#include "stb_image.h"
#include "lite.h"
#include <stdbool.h>

GLuint lite_gl_texture_create(const char* imageFile);

#endif /*LITE_GL_TEXTURE_H*/
