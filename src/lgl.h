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
  GLfloat           *matrix;
} lgl_camera_t;

void lgl_camera_update(unsigned int count, lgl_camera_t *cameras);

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

  lgl_light_t       *lights;
  GLuint             lights_count;

  GLint              render_flags;
} lgl_render_data_t;

DECLARE_LIST(lgl_render_data_t)

typedef struct {
  GLuint             FBO;
  GLuint             RBO;
  GLuint             color_buffers[2];
  lgl_render_data_t  quad;
} lgl_framebuffer_t;

lgl_framebuffer_t lgl_framebuffer_alloc(GLuint shader);

void lgl_active_framebuffer_set(lgl_framebuffer_t* frame);

void lgl_draw(
    const size_t             data_length,
    const lgl_render_data_t *data);

enum {
  LGL_FLAG_ENABLED       = 1,
  LGL_FLAG_USE_STENCIL   = 1 << 1,
  LGL_FLAG_USE_WIREFRAME = 1 << 2,
};

lgl_context_t *lgl_start(const int width, const int height);

void lgl_end_frame();
void lgl_free();
void  lgl_viewport_set(const float width, const float height);

void lgl_outline(
    const size_t         data_length,
    lgl_render_data_t   *data,
    const GLuint         outline_shader,
    const float          thickness);

GLuint  lgl_shader_compile(const char *file_path, GLenum type);
GLuint  lgl_shader_link   (GLuint vertex_shader, GLuint fragment_shader);

lgl_render_data_t lgl_quad_alloc  ();
lgl_render_data_t lgl_cube_alloc  ();

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

