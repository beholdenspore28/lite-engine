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

#include <platform.h>
#include "glad/gl.h"

#include "stb_image.h"
#include "blib/blib_log.h"

#include <stdio.h>
#include <math.h>

typedef struct {
  float          x;
  float          y;
} lgl_2f_t;

typedef struct {
  float          x;
  float          y;
  float          z;
} lgl_3f_t;

typedef struct {
  float          x;
  float          y;
  float          z;
  float          w;
} lgl_4f_t;

typedef struct {
  lgl_3f_t       position;
  lgl_3f_t       normal;
  lgl_2f_t       texture_coordinates;
} lgl_vertex_t;

typedef struct {
  int            type;
  lgl_3f_t       position;
  lgl_3f_t       direction;
  float          cut_off;
  float          outer_cut_off;
  float          constant;
  float          linear;
  float          quadratic;
  lgl_3f_t       diffuse;
  lgl_3f_t       specular;
} lgl_light_t;

typedef struct {
  x_data_t   x_data;  
  int        is_running;
  double     time_current;
  long long  frame_current;
  double     time_delta;
  double     time_last;
  double     time_FPS;
} lgl_context_t;

typedef struct {
  lgl_context_t *context;
  GLuint         VAO;
  GLuint         VBO;
  lgl_vertex_t  *vertices;
  size_t         vertex_count;
  lgl_3f_t       position;
  lgl_3f_t       scale;
  lgl_4f_t       rotation;
  GLuint         shader;
  GLuint         diffuse_map;
  GLuint         specular_map;
  lgl_2f_t       texture_offset;
  lgl_2f_t       texture_scale;
  GLuint         lights_count;
  lgl_light_t   *lights;
  GLint          render_flags;
} lgl_render_data_t;

enum {
  LGL_FLAG_ENABLED       = 1,
  LGL_FLAG_USE_STENCIL   = 1 << 1,
  LGL_FLAG_USE_WIREFRAME = 1 << 2,
};

lgl_context_t lgl_start(void);

void lgl_framebuffer_draw(lgl_render_data_t *frame);
void lgl_end_frame(lgl_context_t *context);
void lgl_free(lgl_context_t *context);
void  lgl_viewport_set(const float width, const float height);

void lgl_outline(
    const size_t       data_length,
    lgl_render_data_t *data,
    const GLuint       outline_shader,
    const float        thickness);

void lgl_draw(const size_t data_length, const lgl_render_data_t *data);

GLuint  lgl_shader_compile(const char *file_path, GLenum type);
GLuint  lgl_shader_link   (GLuint vertex_shader, GLuint fragment_shader);

lgl_render_data_t lgl_quad_alloc  (lgl_context_t *context);
lgl_render_data_t lgl_cube_alloc  (lgl_context_t *context);

void lgl_perspective(
    float *mat,
    const float fov,
    const float aspect,
    const float near,
    const float far);

GLuint lgl_texture_alloc(const char *imageFile);

static inline lgl_2f_t 
lgl_2f_zero   (void)    { return (lgl_2f_t) {  0.0f,  0.0f}; }
static inline lgl_2f_t
lgl_2f_one    (float s) { return (lgl_2f_t) {  s,     s   }; }
static inline lgl_2f_t
lgl_2f_up     (float s) { return (lgl_2f_t) {  0.0f,  s   }; }
static inline lgl_2f_t
lgl_2f_down   (float s) { return (lgl_2f_t) {  0.0f, -s   }; }
static inline lgl_2f_t
lgl_2f_right  (float s) { return (lgl_2f_t) {  s,     0.0f}; }
static inline lgl_2f_t 
lgl_2f_left   (float s) { return (lgl_2f_t) { -s,     0.0f}; }

static inline lgl_3f_t
lgl_3f_zero   (void)    { return (lgl_3f_t) {  0.0f,  0.0f, 0.0f  }; }
static inline lgl_3f_t
lgl_3f_one    (float s) { return (lgl_3f_t) {  s,     s,     s    }; }
static inline lgl_3f_t
lgl_3f_up     (float s) { return (lgl_3f_t) {  0.0f,  s,     0.0f }; }
static inline lgl_3f_t
lgl_3f_down   (float s) { return (lgl_3f_t) {  0.0f, -s,     0.0f }; }
static inline lgl_3f_t
lgl_3f_right  (float s) { return (lgl_3f_t) {  s,     0.0f,  0.0f }; }
static inline lgl_3f_t
lgl_3f_left   (float s) { return (lgl_3f_t) { -s,     0.0f,  0.0f }; }
static inline lgl_3f_t
lgl_3f_forward(float s) { return (lgl_3f_t) {  0.0f,  0.0f,  s    }; }
static inline lgl_3f_t
lgl_3f_back   (float s) { return (lgl_3f_t) {  0.0f,  0.0f, -s    }; }

static inline lgl_4f_t
lgl_4f_zero   (void)    { return (lgl_4f_t){  0.0f,  0.0f,  0.0f, 1.0f }; }
static inline lgl_4f_t
lgl_4f_one    (float s) { return (lgl_4f_t){  s,     s,     s,    1.0f }; }
static inline lgl_4f_t
lgl_4f_up     (float s) { return (lgl_4f_t){  0.0f,  s,     0.0f, 1.0f }; }
static inline lgl_4f_t
lgl_4f_down   (float s) { return (lgl_4f_t){  0.0f, -s,     0.0f, 1.0f }; }
static inline lgl_4f_t
lgl_4f_right  (float s) { return (lgl_4f_t){  s,     0.0f,  0.0f, 1.0f }; }
static inline lgl_4f_t
lgl_4f_left   (float s) { return (lgl_4f_t){ -s,     0.0f,  0.0f, 1.0f }; }
static inline lgl_4f_t
lgl_4f_forward(float s) { return (lgl_4f_t){  0.0f,  0.0f,  s,    1.0f }; }
static inline lgl_4f_t
lgl_4f_back   (float s) { return (lgl_4f_t){  0.0f,  0.0f, -s,    1.0f }; }

static inline float lgl_4f_magnitude(lgl_4f_t q) {
	return sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}

static inline lgl_4f_t lgl_4f_normalize(lgl_4f_t q) {
	float mag = lgl_4f_magnitude(q);
	if (mag == 0) {
		return lgl_4f_zero();
	}
	q.w /= mag;
	q.x /= mag;
	q.y /= mag;
	q.z /= mag;
	return q;
}

static inline lgl_4f_t lgl_4f_conjugate(lgl_4f_t q) {
	return (lgl_4f_t) { 
		.w = q.w, 
    .x = -q.x, 
		.y = -q.y, 
		.z = -q.z 
	};
}

static inline void lgl_4f_print(lgl_4f_t q, const char *label) {
	printf("\t%12f, %12f, %12f, %12f\t%s\n", q.x, q.y, q.z, q.w, label);
}

static inline lgl_4f_t lgl_4f_multiply(lgl_4f_t q1, lgl_4f_t q2) {
	lgl_4f_t ret = {0};
	ret.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	ret.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	ret.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
	ret.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
	return ret;
}

static inline lgl_4f_t lgl_4f_from_euler(lgl_3f_t eulerAngles) {
	lgl_4f_t q;

	float cRoll = cosf(eulerAngles.x * 0.5f);
	float sRoll = sinf(eulerAngles.x * 0.5f);
	float cPitch = cosf(eulerAngles.y * 0.5f);
	float sPitch = sinf(eulerAngles.y * 0.5f);
	float cYaw = cosf(eulerAngles.z * 0.5f);
	float sYaw = sinf(eulerAngles.z * 0.5f);
	q.w = cRoll * cPitch * cYaw + sRoll * sPitch * sYaw;
	q.x = sRoll * cPitch * cYaw - cRoll * sPitch * sYaw;
	q.y = cRoll * sPitch * cYaw + sRoll * cPitch * sYaw;
	q.z = cRoll * cPitch * sYaw - sRoll * sPitch * cYaw;

	return q;
}

static inline float *lgl_4f_to_mat4(lgl_4f_t q, GLfloat *mat) {
	float xx = q.x * q.x;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float xw = q.x * q.w;

	float yy = q.y * q.y;
	float yz = q.y * q.z;
	float yw = q.y * q.w;

	float zz = q.z * q.z;
	float zw = q.z * q.w;

	mat[0]  = 1 - 2 * ( yy + zz );
	mat[4]  =     2 * ( xy - zw );
	mat[8]  =     2 * ( xz + yw );

	mat[1]  =     2 * ( xy + zw );
	mat[5]  = 1 - 2 * ( xx + zz );
	mat[9]  =     2 * ( yz - xw );

	mat[2]  =     2 * ( xz - yw );
	mat[6]  =     2 * ( yz + xw );
	mat[10] = 1 - 2 * ( xx + yy );

	mat[12] = mat[13] = mat[14] = mat[3] = mat[7] = mat[11] = 0;
	mat[15] = 1;

	return mat;
}



#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // LGL_H

