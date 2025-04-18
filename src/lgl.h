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

#include "simple_3D/simple_3D.h"
#include "simple_collections/sc_list.h"
SC_LIST(sv3)
SC_LIST(sv2)

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "blib/blib_log.h"
#include "stb_image.h"

#include "simple_collections/sc_list.h"

#include <math.h>
#include <stdio.h>

SC_LIST(GLuint)

typedef struct {
  sv3 position;
  sv3 normal;
  sv2 texture_coordinates;
} lgl_vertex;
SC_LIST(lgl_vertex)

typedef struct {
  int type;
  sv3 position;
  sv3 direction;
  float cut_off;
  float outer_cut_off;
  float constant;
  float linear;
  float quadratic;
  sv3 diffuse;
  sv3 specular;
} lgl_light;

typedef struct {
  sv3 position;
  sv3 scale;
  sv4 rotation;
} lgl_transform;

void lgl_camera_update(GLfloat *matrix, lgl_transform transform);

typedef struct {
  GLFWwindow *GLFWwindow;
  GLfloat *camera_matrix;
  int is_running;
  double time_current;
  long long frame_current;
  double time_delta;
  double time_last;
  double time_FPS;
} lgl_context;

typedef struct {
  lgl_transform transform;
  GLfloat *matrices;

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  GLuint model_matrix_buffer;

  sc_list_lgl_vertex vertices;
  sc_list_GLuint indices;

  GLuint shader;
  GLuint diffuse_map;
  GLuint specular_map;
  sv4 color;

  lgl_light *lights;
  GLuint lights_count;
  GLenum primitive;
  GLint render_flags;
  unsigned int count;
} lgl_batch;

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

void lgl_draw(const lgl_batch *batch);

enum {
  LGL__FLAGS_BEGIN = 1,
  LGL_FLAG_ENABLED = 1 << 1,
  LGL_FLAG_USE_STENCIL = 1 << 2,
  LGL_FLAG_DRAW_POINTS = 1 << 3,
  LGL_FLAG_USE_WIREFRAME = 1 << 4,
  LGL_FLAG_USE_INSTANCING = 1 << 5,
  LGL__FLAGS_END,

  LGL__PRIMITIVES_BEGIN,
  LGL_PRIMITIVE_TRIANGLES,
  LGL_PRIMITIVE_TRIANGLES_INDEXED,
  LGL_PRIMITIVE_POINTS,
  LGL_PRIMITIVE_LINES,
  LGL__PRIMITIVES_END,

  LGL__ARCHETYPES_BEGIN,
  LGL_ARCHETYPE_EMPTY,
  LGL_ARCHETYPE_CUBE,
  LGL_ARCHETYPE_QUAD,
  LGL_ARCHETYPE_PYRAMID,
  LGL__ARCHETYPES_END,
};

lgl_context *lgl_start(const int width, const int height);
void lgl_update_window_title(void);
void lgl_end_frame(void);
void lgl_free(lgl_context *context);
void lgl_viewport_set(const float width, const float height);

GLuint lgl_shader_compile(const char *file_path, GLenum type);
GLuint lgl_shader_link(GLuint vertex_shader, GLuint fragment_shader);

lgl_batch lgl_batch_alloc(const unsigned int count,
                          const unsigned int archetype);
void lgl_batch_free(lgl_batch batch);
void lgl_lines_alloc(lgl_batch *batch, sc_list_sv3 points);
void lgl_mesh_obj_alloc(lgl_batch *batch, const char *filepath);
void lgl_icosphere_mesh_alloc(lgl_batch *batch,
                              const unsigned int subdivisions);

static inline void lgl_mat4_print(GLfloat *mat) {
  printf("\n-----------------------------\n");
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
