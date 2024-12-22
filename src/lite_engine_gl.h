#ifndef LITE_ENGINE_GL_H
#define LITE_ENGINE_GL_H

#include "lite_engine.h"
#include "glad/glad.h"
#include "blib/blib_math3d.h"

void lite_engine_start_gl(void);
void lite_engine_gl_render(void);
GLuint shader_create(const char *vertexShaderSourcePath, const char *fragmentShaderSourcePath);
void lite_engine_gl_shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i);
void lite_engine_gl_shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f);
void lite_engine_gl_shader_setUniformV3(GLuint shader, const char *uniformName, vector3_t v);
void lite_engine_gl_shader_setUniformV4(GLuint shader, const char *uniformName, vector4_t v);
void lite_engine_gl_shader_setUniformM4(GLuint shader, const char *uniformName, matrix4_t *m);

#endif
