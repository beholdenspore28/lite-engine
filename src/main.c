#include "lal.h"
#include "lgl.h"
#include "physics.h"

#define BLIB_IMPLEMENTATION
#include "blib/blib_math3d.h"

#include <stdio.h>

lgl_context *graphics_context;
GLuint shader_framebuffer = 0;
GLuint shader_solid = 0;
GLuint shader_phong = 0;
lgl_framebuffer framebuffer;
lgl_framebuffer framebuffer_MSAA;
l_object frame_obj;
lgl_light light;

#include "demos/flycam.c"
#include "demos/demo_galaxy.c"
#include "demos/demo_cube.c"
#include "demos/demo_icosphere.c"
#include "demos/demo_physics.c"

int main() {
  alutInit(0, 0);
  graphics_context = lgl_start(1000, 800);

  {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_texture_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_default_fragment.glsl", GL_FRAGMENT_SHADER);
    shader_framebuffer = lgl_shader_link(vertex_shader, fragment_shader);
  }

  {
    GLuint vertex_shader =
        lgl_shader_compile("res/shaders/solid_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/solid_fragment.glsl", GL_FRAGMENT_SHADER);
    shader_solid = lgl_shader_link(vertex_shader, fragment_shader);
  }

  {
    GLuint vertex_shader =
        lgl_shader_compile("res/shaders/phong_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/phong_fragment.glsl", GL_FRAGMENT_SHADER);
    shader_phong = lgl_shader_link(vertex_shader, fragment_shader);
  }

  // --------------------------------------------------------------------------
  // Create framebuffers

  enum {
    SAMPLES = 4,
    NUM_COLOR_BUFFERS = 2,
  };

  int width, height;
  glfwGetFramebufferSize(graphics_context->GLFWwindow, &width, &height);

  frame_obj = l_object_alloc(1);

  framebuffer = lgl_framebuffer_alloc(shader_framebuffer, 1, NUM_COLOR_BUFFERS,
                                      width, height);
  framebuffer_MSAA = lgl_framebuffer_alloc(shader_framebuffer, SAMPLES,
                                           NUM_COLOR_BUFFERS, width, height);

  lgl_active_framebuffer_set(&framebuffer);
  lgl_active_framebuffer_set_MSAA(&framebuffer_MSAA);

  graphics_context->camera = lgl_camera_alloc();
  graphics_context->camera.position.z = -25;

  light = (lgl_light){
      .type = 0,
      .position = {0.0, 0.0, -5},
      .direction = {0.0, 0.0, 1.0},
      .cut_off = cos(12.5),
      .outer_cut_off = cos(15.0),
      .constant = 1.0f,
      .linear = 0.09f,
      .quadratic = 0.032f,
      .diffuse = (vector3){1.0, 1.0, 1.0},
      .specular = vector3_one(0.6),
  };


#if 0
  demo_cube();
#endif

#if 1
  demo_physics();
#endif

#if 0
  demo_icosphere();
#endif

  lgl_camera_free(graphics_context->camera);
  l_object_free(frame_obj);
  lgl_framebuffer_free(framebuffer);
  lgl_framebuffer_free(framebuffer_MSAA);

  lgl_free(graphics_context);

  alutExit();

  return 0;
}
