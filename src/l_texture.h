#ifndef L_TEXTURE_H
#define L_TEXTURE_H

#include "glad/glad.h"

//TODO make a destroy texture func to clean this up
GLuint lite_gl_texture_create(const char* imageFile);

#endif /*L_TEXTURE_H*/
