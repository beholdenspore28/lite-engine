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
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "blib/blib_log.h"
#include "blib/blib_math3d.h"

#include <stdio.h>
#include <math.h>

DECLARE_LIST(GLuint)

typedef struct {
  vector3_t          position;
  vector3_t          normal;
  vector2_t          texture_coordinates;
} lgl_vertex_t;

DECLARE_LIST(lgl_vertex_t)

typedef struct {
  int                type;
  vector3_t          position;
  vector3_t          direction;
  float              cut_off;
  float              outer_cut_off;
  float              constant;
  float              linear;
  float              quadratic;
  vector3_t          diffuse;
  vector3_t          specular;
} lgl_light_t;

typedef struct {
  vector3_t          position;
  quaternion_t       rotation;
  GLfloat           *view;
  GLfloat           *projection;
} lgl_camera_t;

void lgl_camera_update(void);

typedef struct {
  GLFWwindow        *GLFWwindow;
  lgl_camera_t       camera;
  int                is_running;
  double             time_current;
  long long          frame_current;
  double             time_delta;
  double             time_last;
  double             time_FPS;
} lgl_context_t;

typedef struct {
  GLuint             VAO;
  GLuint             VBO;

  list_lgl_vertex_t  vertices;
  
  vector3_t          position;
  vector3_t          scale;
  quaternion_t       rotation;

  GLuint             shader;
  GLuint             diffuse_map;
  GLuint             specular_map;
  vector2_t          texture_offset;
  vector2_t          texture_scale;
  vector4_t          color;

  lgl_light_t       *lights;
  GLuint             lights_count;

  GLint              render_flags;
} lgl_object_t;

DECLARE_LIST(lgl_object_t)

typedef struct {
  GLuint             FBO;
  GLuint             RBO;
  GLuint             color_buffers[2];
  lgl_object_t  quad;
} lgl_framebuffer_t;

void lgl_draw_instanced(
    unsigned int       count,
    lgl_object_t *instance);

lgl_framebuffer_t lgl_framebuffer_alloc(GLuint shader);

void lgl_active_framebuffer_set(lgl_framebuffer_t* frame);

void lgl_draw(
    const size_t             data_length,
    const lgl_object_t *data);

enum {
  LGL_FLAG_ENABLED       = 1,
  LGL_FLAG_USE_STENCIL   = 1 << 1,
  LGL_FLAG_USE_WIREFRAME = 1 << 2,
};

lgl_context_t *lgl_start(const int width, const int height);

void lgl_end_frame(void);
void lgl_free(lgl_context_t *context);
void lgl_viewport_set(const float width, const float height);

void lgl_outline(
    const size_t         data_length,
    lgl_object_t   *data,
    const GLuint         outline_shader,
    const float          thickness);

GLuint  lgl_shader_compile(const char *file_path, GLenum type);
GLuint  lgl_shader_link   (GLuint vertex_shader, GLuint fragment_shader);

lgl_object_t lgl_quad_alloc(void);
lgl_object_t lgl_cube_alloc(void);

static inline void lgl_mat4_print(GLfloat *mat) {
    debug_log("");
    for(int j = 0; j < 4; j++) {
      printf("%.2f\t", mat[j]);
    }
    putchar('\n');
    for(int j = 4; j < 8; j++) {
      printf("%.2f\t", mat[j]);
    }
    putchar('\n');
    for(int j = 8; j < 12; j++) {
      printf("%.2f\t", mat[j]);
    }
    putchar('\n');
    for(int j = 12; j < 16; j++) {
      printf("%.2f\t", mat[j]);
    }
    putchar('\n');
}

static inline void lgl_mat4_identity(GLfloat *m) {
  m[0 ] = 1.0; m[1 ] = 0.0; m[2 ] = 0.0; m[3 ] = 0.0;
  m[4 ] = 0.0; m[5 ] = 1.0; m[6 ] = 0.0; m[7 ] = 0.0;
  m[8 ] = 0.0; m[9 ] = 0.0; m[10] = 1.0; m[11] = 0.0;
  m[12] = 0.0; m[13] = 0.0; m[14] = 0.0; m[15] = 1.0;
}

/*Multiplies a 4x4 matrix with another 4x4 matrix*/
static inline void lgl_mat4_multiply(
    float *result,
    const float *a,
    const float *b) {

  // row 0
  result[ 0] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3];
  result[ 1] = a[ 0] * b[ 1] + a[ 1] * b[ 5] + a[ 2] * b[ 9] + a[ 3] * b[13];
  result[ 2] = a[ 0] * b[ 2] + a[ 1] * b[ 6] + a[ 2] * b[10] + a[ 3] * b[14];
  result[ 3] = a[ 0] * b[ 3] + a[ 1] * b[ 7] + a[ 2] * b[11] + a[ 3] * b[15];

  // row 1
  result[ 4] = a[ 4] * b[ 0] + a[ 5] * b[ 4] + a[ 6] * b[ 8] + a[ 7] * b[12];
  result[ 5] = a[ 4] * b[ 1] + a[ 5] * b[ 5] + a[ 6] * b[ 9] + a[ 7] * b[13];
  result[ 6] = a[ 4] * b[ 2] + a[ 5] * b[ 6] + a[ 6] * b[10] + a[ 7] * b[14];
  result[ 7] = a[ 4] * b[ 3] + a[ 5] * b[ 7] + a[ 6] * b[11] + a[ 7] * b[15];

  // row 2
  result[ 8] = a[ 8] * b[ 0] + a[ 9] * b[ 4] + a[10] * b[ 8] + a[11] * b[12];
  result[ 9] = a[ 8] * b[ 1] + a[ 9] * b[ 5] + a[10] * b[ 9] + a[11] * b[13];
  result[10] = a[ 8] * b[ 2] + a[ 9] * b[ 6] + a[10] * b[10] + a[11] * b[14];
  result[11] = a[ 8] * b[ 3] + a[ 9] * b[ 7] + a[10] * b[11] + a[11] * b[15];

  // row 3
  result[12] = a[12] * b[ 0] + a[13] * b[ 4] + a[14] * b[ 8] + a[15] * b[12];
  result[13] = a[12] * b[ 1] + a[13] * b[ 5] + a[14] * b[ 9] + a[15] * b[13];
  result[14] = a[12] * b[ 2] + a[13] * b[ 6] + a[14] * b[10] + a[15] * b[14];
  result[15] = a[12] * b[ 3] + a[13] * b[ 7] + a[14] * b[11] + a[15] * b[15];
}

void lgl_perspective(
    float *mat,
    const float fov,
    const float aspect,
    const float near,
    const float far);

GLuint lgl_texture_alloc(const char *imageFile);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // LGL_H

