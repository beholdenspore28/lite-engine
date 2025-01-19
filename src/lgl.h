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

enum {
  LGL_FLAG_ENABLED       = 1 << 0, // if not enabled, the renderer will draw this object
  LGL_FLAG_USE_STENCIL   = 1 << 1,
  LGL_FLAG_USE_WIREFRAME = 1 << 2,
};

typedef struct {
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

void  lgl_viewport_set        (const float width, const float height);
void  lgl_draw                (size_t data_length, lgl_render_data_t *data);
void  lgl_buffer_vertex_array (lgl_render_data_t *data);

GLuint  lgl_shader_compile    (const char *file_path, GLenum type);
GLuint  lgl_shader_link       (GLuint vertex_shader, GLuint fragment_shader);

lgl_render_data_t lgl_quad_alloc(void);
lgl_render_data_t lgl_cube_alloc(void);

void lgl_perspective          (float *mat,
                               const float fov,
                               const float aspect,
                               const float near,
                               const float far);

GLuint lgl_texture_alloc(const char *imageFile);

static inline lgl_2f_t lgl_2f_zero    (void)    { return (lgl_2f_t){ 0.0f,  0.0f }; }
static inline lgl_2f_t lgl_2f_one     (float s) { return (lgl_2f_t){ s,     s    }; }
static inline lgl_2f_t lgl_2f_up      (float s) { return (lgl_2f_t){ 0.0f,  s    }; }
static inline lgl_2f_t lgl_2f_down    (float s) { return (lgl_2f_t){ 0.0f, -s    }; }
static inline lgl_2f_t lgl_2f_right   (float s) { return (lgl_2f_t){ s,     0.0f }; }
static inline lgl_2f_t lgl_2f_left    (float s) { return (lgl_2f_t){-s,     0.0f }; }

static inline lgl_3f_t lgl_3f_zero    (void)    { return (lgl_3f_t){ 0.0f,  0.0f, 0.0f  }; }
static inline lgl_3f_t lgl_3f_one     (float s) { return (lgl_3f_t){ s,     s,     s    }; }
static inline lgl_3f_t lgl_3f_up      (float s) { return (lgl_3f_t){ 0.0f,  s,     0.0f }; }
static inline lgl_3f_t lgl_3f_down    (float s) { return (lgl_3f_t){ 0.0f, -s,     0.0f }; }
static inline lgl_3f_t lgl_3f_right   (float s) { return (lgl_3f_t){ s,     0.0f,  0.0f }; }
static inline lgl_3f_t lgl_3f_left    (float s) { return (lgl_3f_t){-s,     0.0f,  0.0f }; }
static inline lgl_3f_t lgl_3f_forward (float s) { return (lgl_3f_t){ 0.0f,  0.0f,  s    }; }
static inline lgl_3f_t lgl_3f_back    (float s) { return (lgl_3f_t){ 0.0f,  0.0f, -s    }; }

static inline lgl_4f_t lgl_4f_zero    (void)    { return (lgl_4f_t){ 0.0f,  0.0f,  0.0f, 1.0f }; }
static inline lgl_4f_t lgl_4f_one     (float s) { return (lgl_4f_t){ s,     s,     s,    1.0f }; }
static inline lgl_4f_t lgl_4f_up      (float s) { return (lgl_4f_t){ 0.0f,  s,     0.0f, 1.0f }; }
static inline lgl_4f_t lgl_4f_down    (float s) { return (lgl_4f_t){ 0.0f, -s,     0.0f, 1.0f }; }
static inline lgl_4f_t lgl_4f_right   (float s) { return (lgl_4f_t){ s,     0.0f,  0.0f, 1.0f }; }
static inline lgl_4f_t lgl_4f_left    (float s) { return (lgl_4f_t){-s,     0.0f,  0.0f, 1.0f }; }
static inline lgl_4f_t lgl_4f_forward (float s) { return (lgl_4f_t){ 0.0f,  0.0f,  s,    1.0f }; }
static inline lgl_4f_t lgl_4f_back    (float s) { return (lgl_4f_t){ 0.0f,  0.0f, -s,    1.0f }; }


#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif // LGL_H

