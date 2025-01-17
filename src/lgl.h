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
	float		x;
	float		y;
} lgl_2f_t;

typedef struct {
	float		x;
	float		y;
	float		z;
} lgl_3f_t;

typedef struct {
	float		x;
	float		y;
	float		z;
	float		w;
} lgl_4f_t;

typedef struct {
	lgl_3f_t	position;
	lgl_2f_t	texture_coordinates;
} lgl_vertex_t;

typedef struct {
	int		type;
	lgl_3f_t	position;
	lgl_3f_t	direction;
	float		cut_off;
	float		outer_cut_off;
	float		constant;
	float		linear;
	float		quadratic;
	lgl_3f_t	diffuse;
	lgl_3f_t	specular;
} lgl_light_t;

// TODO perhaps some sort of spatial partitioning to help with
// supporting more lights (just an idea)
enum { LGL_MAX_LIGHTS = 8 };

typedef struct {
	GLuint		VAO;
	GLuint		VBO;
	lgl_vertex_t	*vertices;
	size_t		vertex_count;
	lgl_3f_t	position;
	lgl_3f_t	scale;
	lgl_4f_t	rotation;
	GLuint		shader;
	GLuint		diffuseMap;
	GLuint		specularMap;
	lgl_light_t	*lights;
} lgl_render_data_t;

void	lgl_viewport_set	(const float width, const float height);
void	lgl_draw		(size_t data_length, lgl_render_data_t *data);
void	lgl_buffer_vertex_array	(lgl_render_data_t *data);

GLuint	lgl_shader_compile	(const char *file_path, GLenum type);
GLuint	lgl_shader_link		(GLuint vertex_shader, GLuint fragment_shader);

lgl_render_data_t lgl_quad_alloc(void);
lgl_render_data_t lgl_cube_alloc(void);

void lgl_perspective(float *mat,
		const float fov,
		const float aspect,
		const float near,
		const float far);

GLuint lgl_texture_alloc(const char *imageFile);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // LGL_H

