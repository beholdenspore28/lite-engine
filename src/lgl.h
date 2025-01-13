/*--------------------------------------------------------------------------/
/                                                                           /
/ lgl.h                                                                     /
/ A light-weight OpenGL renderer                                            /
/                                                                           /
/--------------------------------------------------------------------------*/

#ifndef LGL_H
#define LGL_H

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

#include "glad/gl.h"
#include "glad/glx.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "stb_image.h"
#include "blib/blib_log.h"

typedef struct {
	float	x;
	float	y;
	float	z;
	float	u;
	float	v;
} lgl_vertex_t;

typedef struct {
	GLuint		VAO;
	GLuint		VBO;
	lgl_vertex_t	*vertices;
	size_t		vertex_count;
	float		position_x, position_y, position_z;
	float		scale_x, scale_y, scale_z;
	float		rotation_i, rotation_j, rotation_k, rotation_r;
	GLuint		shader;
	GLuint		diffuseMap;
	GLuint		specularMap;
} lgl_render_data_t;

void			lgl_viewport_set(const float width, const float height);
void			lgl_draw(size_t data_length, lgl_render_data_t *data);
void			lgl_buffer_vertex_array(lgl_render_data_t *data);

GLuint			lgl_shader_compile(const char *file_path, GLenum type);
GLuint			lgl_shader_link(GLuint vertex_shader, GLuint fragment_shader);

lgl_render_data_t	lgl_quad_alloc(void);
lgl_render_data_t	lgl_cube_alloc(void);

void			lgl_perspective(float *mat,
					const float fov,
					const float aspect,
					const float near,
					const float far);

GLuint lgl_texture_alloc(const char *imageFile);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // LGL_H

