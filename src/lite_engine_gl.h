#ifndef LITE_ENGINE_GL_H
#define LITE_ENGINE_GL_H

#include "blib/blib_file.h"
#include "blib/blib_math3d.h"
#include "glad/glad.h"
#include "lite_engine.h"

DECLARE_LIST(GLuint)

typedef struct {
	transform_t *transforms;
	mesh_t	    *meshes;
	light_t	    *lights;
	material_t  *materials;
	camera_t    *cameras;
} lite_engine_gl_state_t;

void lite_engine_gl_set_state(lite_engine_gl_state_t state);
void lite_engine_gl_start(void);
void lite_engine_gl_stop(void);
void lite_engine_gl_render(void);

GLuint lite_engine_gl_texture_create(const char *imageFile);

void lite_engine_gl_transform_rotate(uint64_t entity, quaternion_t rotation);
void lite_engine_gl_transform_calculate_matrix(transform_t *t);
void lite_engine_gl_transform_calculate_view_matrix(transform_t *t);
vector3_t lite_engine_gl_transform_basis_forward(transform_t t,
						 float	     magnitude);
vector3_t lite_engine_gl_transform_basis_up(transform_t t, float magnitude);
vector3_t lite_engine_gl_transform_basis_right(transform_t t, float magnitude);
vector3_t lite_engine_gl_transform_basis_back(transform_t t, float magnitude);
vector3_t lite_engine_gl_transform_basis_down(transform_t t, float magnitude);
vector3_t lite_engine_gl_transform_basis_left(transform_t t, float magnitude);

mesh_t lite_engine_gl_mesh_alloc(list_vertex_t vertices, list_ui32 indices);
mesh_t lite_engine_gl_mesh_lmod_alloc(const char *file_path);
void   lite_engine_gl_mesh_free(mesh_t *mesh);
void   lite_engine_gl_mesh_update(lite_engine_gl_state_t state);

GLuint lite_engine_gl_shader_create(const char *vertex_shader_file_path,
				    const char *fragment_shader_file_path);

void lite_engine_gl_shader_setUniformInt(GLuint shader, const char *uniformName,
					 GLuint i);
void lite_engine_gl_shader_setUniformFloat(GLuint      shader,
					   const char *uniformName, GLfloat f);
void lite_engine_gl_shader_setUniformV3(GLuint shader, const char *uniformName,
					vector3_t v);
void lite_engine_gl_shader_setUniformV4(GLuint shader, const char *uniformName,
					vector4_t v);
void lite_engine_gl_shader_setUniformM4(GLuint shader, const char *uniformName,
					matrix4_t *m);

ui64 lite_engine_gl_get_active_camera(void);
void lite_engine_gl_set_active_camera(ui64 camera);
void lite_engine_gl_set_prefer_window_title(char *title);
void lite_engine_gl_set_prefer_window_size_x(ui16 size_x);
void lite_engine_gl_set_prefer_window_size_y(ui16 size_y);

void lite_engine_gl_set_prefer_window_size(ui16 size_x, ui16 size_y);
void lite_engine_gl_set_prefer_window_position(ui16 pos_x, ui16 pos_y);

void lite_engine_gl_set_prefer_window_position_x(ui16 pos_x);
void lite_engine_gl_set_prefer_window_position_y(ui16 pos_y);
void lite_engine_gl_set_prefer_window_always_on_top(ui8 always_on_top);
void lite_engine_gl_set_prefer_window_fullscreen(ui8 fullscreen);

#endif
