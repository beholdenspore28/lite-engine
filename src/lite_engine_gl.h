#ifndef LITE_ENGINE_GL_H
#define LITE_ENGINE_GL_H

#include "lite_engine.h"
#include "glad/glad.h"
#include "blib/blib_math3d.h"

void lite_engine_start_gl(void);
void lite_engine_gl_render(void);

GLuint lite_engine_gl_texture_create(const char *imageFile);

GLuint lite_engine_gl_shader_create( const char *vertex_shader_file_path, const char *fragment_shader_file_path);
void lite_engine_gl_shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i);
void lite_engine_gl_shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f);
void lite_engine_gl_shader_setUniformV3(GLuint shader, const char *uniformName, vector3_t v);
void lite_engine_gl_shader_setUniformV4(GLuint shader, const char *uniformName, vector4_t v);
void lite_engine_gl_shader_setUniformM4(GLuint shader, const char *uniformName, matrix4_t *m);

void lite_engine_gl_set_prefer_window_title(char *title);
void lite_engine_gl_set_prefer_window_size_x(ui16 size_x);
void lite_engine_gl_set_prefer_window_size_y(ui16 size_y);
void lite_engine_gl_set_prefer_window_position_x(ui16 pos_x);
void lite_engine_gl_set_prefer_window_position_y(ui16 pos_y);
void lite_engine_gl_set_prefer_window_always_on_top(ui8 always_on_top);
void lite_engine_gl_set_prefer_window_fullscreen(ui8 fullscreen);

#endif
