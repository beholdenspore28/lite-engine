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

#include "engine.h"

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "blib/blib.h"
#include "blib/blib_log.h"
#include "blib/blib_math3d.h"
#include "stb_image.h"

#include <math.h>
#include <stdio.h>

DECLARE_LIST(GLuint)

typedef struct {
  vector3 position;
  vector3 normal;
  vector2 texture_coordinates;
} lgl_vertex;

DECLARE_LIST(lgl_vertex)

typedef struct {
  int type;
  vector3 position;
  vector3 direction;
  float cut_off;
  float outer_cut_off;
  float constant;
  float linear;
  float quadratic;
  vector3 diffuse;
  vector3 specular;
} lgl_light;

typedef struct {
  vector3 position;
  quaternion rotation;
  GLfloat *view;
  GLfloat *projection;
} lgl_camera;

lgl_camera lgl_camera_alloc(void);
void lgl_camera_free(lgl_camera camera);
void lgl_camera_update(void);

typedef struct {
  GLFWwindow *GLFWwindow;
  lgl_camera camera;
  int is_running;
  double time_current;
  long long frame_current;
  double time_delta;
  double time_last;
  double time_FPS;
} lgl_context;

typedef struct {
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

  list_lgl_vertex vertices;
  list_GLuint indices;

  GLuint model_matrix_buffer;

  GLuint shader;
  GLuint diffuse_map;
  GLuint specular_map;
  vector2 texture_offset;
  vector2 texture_scale;
  vector4 color;

  lgl_light *lights;
  GLuint lights_count;
  GLint render_flags;
} lgl_batch;

DECLARE_LIST(lgl_batch)

typedef struct {
  GLuint FBO;
  GLuint RBO;
  GLuint *color_buffers;
  GLuint color_buffers_count;
  GLuint width;
  GLuint height;
  GLuint samples;
  lgl_batch quad;
} lgl_framebuffer;

lgl_framebuffer lgl_framebuffer_alloc(GLuint shader, GLuint samples,
                                      GLuint num_color_attachments,
                                      GLuint width, GLuint height);

void lgl_framebuffer_free(lgl_framebuffer frame);

void lgl_active_framebuffer_set(lgl_framebuffer *frame);
void lgl_active_framebuffer_set_MSAA(lgl_framebuffer *frame);

void lgl_draw(l_object object, const lgl_batch batch);
void lgl_draw_instanced(l_object object, const lgl_batch batch);

enum {
  LGL_FLAG_ENABLED = 1,
  LGL_FLAG_USE_STENCIL = 1 << 1,
  LGL_FLAG_USE_WIREFRAME = 1 << 2,
  LGL_FLAG_DRAW_POINTS = 1 << 3, // tells the renderer to draw a black dot at
                                 // the location of the vertex
  LGL_FLAG_INDEXED_DRAW =
      1 << 4, // tells the renderer to use the mesh's index array
};

lgl_context *lgl_start(const int width, const int height);
void lgl_update_window_title(void);
void lgl_end_frame(void);
void lgl_free(lgl_context *context);
void lgl_viewport_set(const float width, const float height);

GLuint lgl_shader_compile(const char *file_path, GLenum type);
GLuint lgl_shader_link(GLuint vertex_shader, GLuint fragment_shader);

lgl_batch lgl_batch_alloc(unsigned int count, unsigned int archetype);
void lgl_batch_free(lgl_batch batch);
void lgl_icosphere_mesh_alloc(lgl_batch *batch,
                              const unsigned int subdivisions);

void lgl_mat4_buffer(l_object object, lgl_batch *batch);

static inline void lgl_mat4_print(GLfloat *mat) {
  debug_log("");
  for (int j = 0; j < 4; j++) {
    printf("%.2f\t", mat[j]);
  }
  putchar('\n');
  for (int j = 4; j < 8; j++) {
    printf("%.2f\t", mat[j]);
  }
  putchar('\n');
  for (int j = 8; j < 12; j++) {
    printf("%.2f\t", mat[j]);
  }
  putchar('\n');
  for (int j = 12; j < 16; j++) {
    printf("%.2f\t", mat[j]);
  }
  putchar('\n');
}

static inline void lgl_mat4_identity(GLfloat *m) {
  m[0] = 1.0;
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;
  m[4] = 0.0;
  m[5] = 1.0;
  m[6] = 0.0;
  m[7] = 0.0;
  m[8] = 0.0;
  m[9] = 0.0;
  m[10] = 1.0;
  m[11] = 0.0;
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = 0.0;
  m[15] = 1.0;
}

/*Multiplies a 4x4 matrix with another 4x4 matrix*/
static inline void lgl_mat4_multiply(float *result, const float *a,
                                     const float *b) {

  // row 0
  result[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
  result[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
  result[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
  result[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];

  // row 1
  result[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
  result[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
  result[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
  result[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];

  // row 2
  result[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
  result[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
  result[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
  result[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];

  // row 3
  result[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
  result[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
  result[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
  result[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
}

void lgl_perspective(float *mat, const float fov, const float aspect,
                     const float near, const float far);

GLuint lgl_texture_alloc(const char *imageFile);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // LGL_H
