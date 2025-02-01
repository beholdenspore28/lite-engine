#include "platform.h"
#include "lgl.h"

#include <time.h>

void lgl__viewport_size_callback(
    const unsigned int width,
    const unsigned int height) {
  glViewport(0, 0, width, height);
}

// initializes lite-engine. call this to rev up those fryers!
lgl_context_t lgl_start(void) {
  debug_log("Rev up those fryers!");

  lgl_context_t context = (lgl_context_t) {
    .is_running     = 1,
    .time_current   = 0,
    .frame_current  = 0,
    .time_delta     = 0,
    .time_last      = 0,
    .time_FPS       = 0,
    .x_data         = x_start("Game Window", 640, 480),
  };

  context.x_data.viewport_size_callback = lgl__viewport_size_callback;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glStencilOp   (GL_KEEP, GL_KEEP, GL_REPLACE);
  glStencilFunc (GL_ALWAYS, 1, 0xFF);

  debug_log("Startup completed successfuly");

  return context;
}

void lgl__time_update(lgl_context_t *context) {
  struct timespec spec;
  if (clock_gettime(CLOCK_MONOTONIC, &spec) != 0) {
    debug_error("failed to get time spec.");
    exit(0);
  }

  context->time_current  = spec.tv_sec + spec.tv_nsec * 1e-9;
  context->time_delta    = context->time_current - context->time_last;
  context->time_last     = context->time_current;
  context->time_FPS      = 1 / context->time_delta;
  context->frame_current++;

#if 0 // log time
  debug_log( "\n"
    "time_current:   %lf\n"
    "frame_current:  %lu\n"
    "time_delta:     %lf\n"
    "time_last:      %lf\n"
    "time_FPS:       %lf",
    context->time_current,
    context->frame_current,
    context->time_delta,
    context->time_last,
    context->time_FPS);
#endif // log time
}

// shut down and free all memory associated with the lite-engine context
void lgl_free(lgl_context_t *context) {
  debug_log("Shutting down...");

  context->is_running = 0;

  x_stop   (&context->x_data);
  free     (context);
  debug_log("Shutdown complete");
}

void lgl_end_frame(lgl_context_t *context) {
  x_end_frame(&context->x_data);
  lgl__time_update(context);
}


int main() {
  lgl_context_t context = lgl_start();

  GLuint shader_phong = 0; {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/phong_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/phong_fragment.glsl",
        GL_FRAGMENT_SHADER);

    shader_phong = lgl_shader_link(vertex_shader, fragment_shader);
  }

  GLuint shader_solid = 0; {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/solid_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/solid_fragment.glsl",
        GL_FRAGMENT_SHADER);

    shader_solid = lgl_shader_link(vertex_shader, fragment_shader);
  }

  enum {
    LIGHTS_POINT_0,
    LIGHTS_POINT_1,
    LIGHTS_COUNT, // this should ALWAYS be at the end of the enum
  };
  lgl_light_t lights [LIGHTS_COUNT] = {0};

  enum {
    OBJECTS_FLOOR,
    OBJECTS_CUBE,
    OBJECTS_COUNT  // this should ALWAYS be at the end of the enum,
  };
  lgl_render_data_t objects [OBJECTS_COUNT] = {0};

  lights[LIGHTS_POINT_0] = (lgl_light_t) {
    .type           = 0,
    .position       = {0.0, 1.0, -5.0},
    .direction      = {0.0, 0.0, 1.0},
    .cut_off        = cos(12.5),
    .outer_cut_off  = cos(15.0),
    .constant       = 1.0f,
    .linear         = 0.09f,
    .quadratic      = 0.032f,
    .diffuse        = (lgl_3f_t){1.0, 0.0, 0.0},
    .specular       = lgl_3f_one(0.6),
  };

  lights[LIGHTS_POINT_1] = (lgl_light_t) {
    .type           = 0,
    .position       = {0.0, 1.0, -5.0},
    .direction      = {0.0, 0.0, 1.0},
    .cut_off        = cos(12.5),
    .outer_cut_off  = cos(15.0),
    .constant       = 1.0f,
    .linear         = 0.09f,
    .quadratic      = 0.032f,
    .diffuse        = (lgl_3f_t){0.0, 0.0, 1.0},
    .specular       = lgl_3f_one(0.6),
  };

  GLuint
    texture_diffuse  = lgl_texture_alloc("res/textures/test.png"),
    texture_cube     = lgl_texture_alloc("res/textures/lite-engine-cube.png"),
    texture_specular = lgl_texture_alloc("res/textures/default_specular.png");

  objects[OBJECTS_FLOOR] = lgl_cube_alloc(&context); {
    objects[OBJECTS_FLOOR].shader        =  shader_phong;
    objects[OBJECTS_FLOOR].diffuse_map   =  texture_diffuse;
    objects[OBJECTS_FLOOR].specular_map  =  texture_specular;
    objects[OBJECTS_FLOOR].texture_scale =  lgl_2f_one(10.0);
    objects[OBJECTS_FLOOR].position.y    = -1;
    objects[OBJECTS_FLOOR].scale         =  (lgl_3f_t) {10, 1, 10};
    objects[OBJECTS_FLOOR].lights_count  =  LIGHTS_COUNT;
    objects[OBJECTS_FLOOR].lights        =  lights;
  }

  objects[OBJECTS_CUBE] = lgl_cube_alloc(&context); {
    objects[OBJECTS_CUBE].shader         =  shader_phong;
    objects[OBJECTS_CUBE].diffuse_map    =  texture_cube;
    objects[OBJECTS_CUBE].position.z     =  1;
    objects[OBJECTS_CUBE].lights_count   =  LIGHTS_COUNT;
    objects[OBJECTS_CUBE].lights         =  lights;
    objects[OBJECTS_CUBE].render_flags  |=  LGL_FLAG_USE_STENCIL;
  }

  while(context.is_running) {
    { // update
      objects[OBJECTS_CUBE].position.y = cos(context.time_current)*0.2 + 0.5;

      lights[LIGHTS_POINT_0].position.x = sin(context.time_current);
      lights[LIGHTS_POINT_0].position.z = cos(context.time_current);
      lights[LIGHTS_POINT_1].position.x = cos(context.time_current);
      lights[LIGHTS_POINT_1].position.z = sin(context.time_current);
    }

    { // draw scene to the frame
      glClearColor(0,0,0,1);
      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw(OBJECTS_COUNT, objects);
      lgl_outline(1, &objects[OBJECTS_CUBE], shader_solid, 0.01);
      lgl_end_frame(&context);
    }
  }

  lgl_free(&context);

  return 0;
}
