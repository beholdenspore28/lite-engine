/*--------------------------------------------------------------------------/
  /                                                                           /
  / lgl.h                                                                     /
  / A light-weight OpenGL renderer                                            /
  /                                                                           /
  /--------------------------------------------------------------------------*/

#ifndef renderer_gl_H
#define renderer_gl_H

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
} renderer_gl_vertex;
SC_LIST(renderer_gl_vertex)

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
} renderer_gl_light;

typedef struct {
  vector3 position;
  vector3 scale;
  vector4 rotation;
} renderer_gl_transform;

void renderer_gl_camera_update(GLfloat *matrix, renderer_gl_transform transform);

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
} renderer_gl_context;

typedef struct {
  renderer_gl_transform *transform;
  GLfloat *matrices;

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  GLuint model_matrix_buffer;

  sc_list_renderer_gl_vertex vertices;
  sc_list_GLuint indices;

  GLuint shader;
  GLuint diffuse_map;
  GLuint specular_map;
  vector4 color;

  renderer_gl_light *lights;
  GLuint lights_count;
  GLenum primitive;
  GLint render_flags;
  unsigned int count;
} renderer_gl_batch;

typedef struct {
  GLuint FBO;
  GLuint RBO;
  GLuint *color_buffers;
  GLuint color_buffers_count;
  GLuint width;
  GLuint height;
  GLuint samples;
  renderer_gl_batch quad;
} renderer_gl_framebuffer;

renderer_gl_framebuffer renderer_gl_framebuffer_alloc(GLuint shader, GLuint samples,
                                      GLuint num_color_attachments,
                                      GLuint width, GLuint height);

void renderer_gl_framebuffer_free(renderer_gl_framebuffer frame);

void renderer_gl_active_framebuffer_set(renderer_gl_framebuffer *frame);
void renderer_gl_active_framebuffer_set_MSAA(renderer_gl_framebuffer *frame);

void renderer_gl_draw(const renderer_gl_batch *batch);

enum {
  RENDERER_GL__FLAGS_BEGIN = 1,
  RENDERER_GL_FLAG_ENABLED = 1 << 1,
  RENDERER_GL_FLAG_USE_STENCIL = 1 << 2,
  RENDERER_GL_FLAG_DRAW_POINTS = 1 << 3,
  RENDERER_GL_FLAG_USE_WIREFRAME = 1 << 4,
  RENDERER_GL_FLAG_USE_INSTANCING = 1 << 5,
  RENDERER_GL__FLAGS_END,

  RENDERER_GL__PRIMITIVES_BEGIN,
  RENDERER_GL_PRIMITIVE_TRIANGLES,
  RENDERER_GL_PRIMITIVE_TRIANGLES_INDEXED,
  RENDERER_GL_PRIMITIVE_POINTS,
  RENDERER_GL_PRIMITIVE_LINES,
  RENDERER_GL__PRIMITIVES_END,

  RENDERER_GL__ARCHETYPES_BEGIN,
  RENDERER_GL_ARCHETYPE_EMPTY,
  RENDERER_GL_ARCHETYPE_CUBE,
  RENDERER_GL_ARCHETYPE_QUAD,
  RENDERER_GL_ARCHETYPE_PYRAMID,
  RENDERER_GL__ARCHETYPES_END,
};

renderer_gl_context *renderer_gl_start(const int width, const int height);
void renderer_gl_update_window_title(void);
void renderer_gl_end_frame(void);
void renderer_gl_free(renderer_gl_context *context);
void renderer_gl_viewport_set(const GLfloat width, const GLfloat height);

GLuint renderer_gl_shader_compile(const char *file_path, GLenum type);
GLuint renderer_gl_shader_link(GLuint vertex_shader, GLuint fragment_shader);

renderer_gl_batch renderer_gl_batch_alloc(const unsigned int count,
                          const unsigned int archetype);

void renderer_gl_batch_free(renderer_gl_batch batch);
void renderer_gl_lines_alloc(renderer_gl_batch *batch, sc_list_vector3 points);
void renderer_gl_mesh_obj_alloc(renderer_gl_batch *batch, const char *filepath);

void renderer_gl_icosphere_mesh_alloc(renderer_gl_batch *batch,
                              const unsigned int subdivisions);

void renderer_gl_perspective(GLfloat *mat, const GLfloat fov, const GLfloat aspect,
                     const GLfloat near, const GLfloat far);

GLuint renderer_gl_texture_alloc(const char *imageFile);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // renderer_gl_H
