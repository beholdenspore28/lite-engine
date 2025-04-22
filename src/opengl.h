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

#include "math3d.h"
#include "collections.h"

SC_LIST(vector3)
SC_LIST(vector2)

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "log.h"
#include "stb_image.h"

#include "collections.h"

#include <math.h>
#include <stdio.h>

SC_LIST(GLuint)

typedef struct {
  vector3 position;
  vector3 normal;
  vector2 texture_coordinates;
} lgl_vertex;
SC_LIST(lgl_vertex)

typedef struct {
  int type;
  vector3 position;
  vector3 direction;
  GLfloat cut_off;
  GLfloat outer_cut_off;
  GLfloat constant;
  GLfloat linear;
  GLfloat quadratic;
  vector3 diffuse;
  vector3 specular;
} lgl_light;

typedef struct {
  vector3 position;
  vector3 scale;
  vector4 rotation;
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
  unsigned int draw_calls;
} lgl_context;

typedef struct {
  lgl_transform *transform;
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
  vector4 color;

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
void lgl_viewport_set(const GLfloat width, const GLfloat height);

GLuint lgl_shader_compile(const char *file_path, GLenum type);
GLuint lgl_shader_link(GLuint vertex_shader, GLuint fragment_shader);

lgl_batch lgl_batch_alloc(const unsigned int count,
                          const unsigned int archetype);

void lgl_batch_free(lgl_batch batch);
void lgl_lines_alloc(lgl_batch *batch, sc_list_vector3 points);
void lgl_mesh_obj_alloc(lgl_batch *batch, const char *filepath);

void lgl_icosphere_mesh_alloc(lgl_batch *batch,
                              const unsigned int subdivisions);

void lgl_perspective(GLfloat *mat, const GLfloat fov, const GLfloat aspect,
                     const GLfloat near, const GLfloat far);

GLuint lgl_texture_alloc(const char *imageFile);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // LGL_H
