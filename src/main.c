
// DEMOS HERE. move the demo you want to see to the bottom of
// this stack of define directives then compile the project. it should load your
// chosen demo scene.
#define DEMO demo_icosphere() // procedural icosphere
#define DEMO demo_galaxy()    // GPU instancing many static objects
#define DEMO demo_cube()      // just a spinning cube with a 3D sound
#define DEMO demo_physics()   // GPU instancing with dynamic particle system

#include "lal.h"
#include "lgl.h"

#define BLIB_IMPLEMENTATION
#include "blib/blib_math3d.h"
#undef BLIB_IMPLEMENTATION

lgl_context *graphics_context;
lgl_light light;

#include "demo_shaders.inc"
#include "demo_framebuffer.inc"
#include "demo_flycam.inc"
#include "demo_cube.inc"
#include "demo_galaxy.inc"
#include "demo_icosphere.inc"
#include "demo_physics.inc"

int main() {
  alutInit(0, 0);
  graphics_context = lgl_start(1000, 800);

  demo_shaders_load();
  demo_framebuffer_alloc();

  // --------------------------------------------------------------------------
  // lights

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

  // --------------------------------------------------------------------------
  // camera

  graphics_context->camera = lgl_camera_alloc();
  graphics_context->camera.position.z = -25;

  // --------------------------------------------------------------------------
  // action

  DEMO;

  l_object_free(frame_obj);
  lgl_framebuffer_free(framebuffer);
  lgl_framebuffer_free(framebuffer_MSAA);
  lgl_free(graphics_context);
  alutExit();

  return 0;
}
