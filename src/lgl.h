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
  vector3_t       position;
  vector3_t       normal;
  vector2_t       texture_coordinates;
} lgl_vertex_t;
DECLARE_LIST(lgl_vertex_t)

typedef struct {
  int            type;
  vector3_t       position;
  vector3_t       direction;
  float          cut_off;
  float          outer_cut_off;
  float          constant;
  float          linear;
  float          quadratic;
  vector3_t       diffuse;
  vector3_t       specular;
} lgl_light_t;

typedef struct {
  GLFWwindow* GLFWwindow;
  int        is_running;
  double     time_current;
  long long  frame_current;
  double     time_delta;
  double     time_last;
  double     time_FPS;
} lgl_context_t;

typedef struct {
  GLuint         VAO;
  GLuint         VBO;
  lgl_vertex_t  *vertices;
  size_t         vertex_count;
  vector3_t      position;
  vector3_t      scale;
  quaternion_t   rotation;
  GLuint         shader;
  GLuint         diffuse_map;
  GLuint         specular_map;
  vector2_t      texture_offset;
  vector2_t      texture_scale;
  GLuint         lights_count;
  lgl_light_t   *lights;
  GLint          render_flags;
} lgl_render_data_t;

typedef struct {
  GLuint            FBO;
  GLuint            color_buffers[2];
  lgl_render_data_t quad;
} lgl_framebuffer_t;

void lgl_framebuffer_alloc(
    lgl_framebuffer_t *frame,
    GLuint             shader);

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

//static inline vector2_t vector2_zero   (void)    { return (vector2_t) {  0.0f,  0.0f}; }
//static inline vector2_t vector2_one    (float s) { return (vector2_t) {  s,     s   }; }
//static inline vector2_t vector2_up     (float s) { return (vector2_t) {  0.0f,  s   }; }
//static inline vector2_t vector2_down   (float s) { return (vector2_t) {  0.0f, -s   }; }
//static inline vector2_t vector2_right  (float s) { return (vector2_t) {  s,     0.0f}; }
//static inline vector2_t vector2_left   (float s) { return (vector2_t) { -s,     0.0f}; }
//
//static inline vector3_t vector3_zero   (void)    { return (vector3_t) {  0.0f,  0.0f, 0.0f  }; }
//static inline vector3_t vector3_one    (float s) { return (vector3_t) {  s,     s,     s    }; }
//static inline vector3_t vector3_up     (float s) { return (vector3_t) {  0.0f,  s,     0.0f }; }
//static inline vector3_t vector3_down   (float s) { return (vector3_t) {  0.0f, -s,     0.0f }; }
//static inline vector3_t vector3_right  (float s) { return (vector3_t) {  s,     0.0f,  0.0f }; }
//static inline vector3_t vector3_left   (float s) { return (vector3_t) { -s,     0.0f,  0.0f }; }
//static inline vector3_t vector3_forward(float s) { return (vector3_t) {  0.0f,  0.0f,  s    }; }
//static inline vector3_t vector3_back   (float s) { return (vector3_t) {  0.0f,  0.0f, -s    }; }
//
//static inline vector4_t vector4_zero   (void)    { return (vector4_t){  0.0f,  0.0f,  0.0f, 1.0f }; }
//static inline vector4_t vector4_one    (float s) { return (vector4_t){  s,     s,     s,    1.0f }; }
//static inline vector4_t vector4_up     (float s) { return (vector4_t){  0.0f,  s,     0.0f, 1.0f }; }
//static inline vector4_t vector4_down   (float s) { return (vector4_t){  0.0f, -s,     0.0f, 1.0f }; }
//static inline vector4_t vector4_right  (float s) { return (vector4_t){  s,     0.0f,  0.0f, 1.0f }; }
//static inline vector4_t vector4_left   (float s) { return (vector4_t){ -s,     0.0f,  0.0f, 1.0f }; }
//static inline vector4_t vector4_forward(float s) { return (vector4_t){  0.0f,  0.0f,  s,    1.0f }; }
//static inline vector4_t vector4_back   (float s) { return (vector4_t){  0.0f,  0.0f, -s,    1.0f }; }

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // LGL_H

