#include "lite_engine.h"
#include "platform_x11.h"
#include "lgl.h"

int main() {
  lite_engine_context_t *engine = lite_engine_start();

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
    .position       = {0.0, 5.0 -5.0},
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
    .position       = {0.0, 5.0 -5.0},
    .direction      = {0.0, 0.0, 1.0},
    .cut_off        = cos(12.5),
    .outer_cut_off  = cos(15.0),
    .constant       = 1.0f,
    .linear         = 0.09f,
    .quadratic      = 0.032f,
    .diffuse        = (lgl_3f_t){1.0, 0.0, 0.0},
    .specular       = lgl_3f_one(0.6),
  };

  objects[OBJECTS_FLOOR] = lgl_cube_alloc(); {
    GLuint vertex_shader   = lgl_shader_compile("res/shaders/phong_diffuse_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile("res/shaders/phong_diffuse_fragment.glsl", GL_FRAGMENT_SHADER);
    objects[OBJECTS_FLOOR].shader = lgl_shader_link(vertex_shader, fragment_shader);

    objects[OBJECTS_FLOOR].diffuse_map   = lgl_texture_alloc("res/textures/wood1.jpg");
    objects[OBJECTS_FLOOR].specular_map  = lgl_texture_alloc("res/textures/default_specular.png");
    objects[OBJECTS_FLOOR].texture_scale = lgl_2f_one(10.0);

    objects[OBJECTS_FLOOR].position.y    = -1;
    objects[OBJECTS_FLOOR].scale         = (lgl_3f_t) {10, 1, 10};
    objects[OBJECTS_FLOOR].lights_count = LIGHTS_COUNT;
    objects[OBJECTS_FLOOR].lights = lights;
  }

  objects[OBJECTS_CUBE] = lgl_cube_alloc(); {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/phong_diffuse_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/phong_diffuse_fragment.glsl",
        GL_FRAGMENT_SHADER);

    objects[OBJECTS_CUBE].shader         = lgl_shader_link(vertex_shader, fragment_shader);
    objects[OBJECTS_CUBE].diffuse_map    = lgl_texture_alloc("res/textures/lite-engine-cube.png");
    objects[OBJECTS_CUBE].position.z     = 2;
    objects[OBJECTS_CUBE].lights_count = LIGHTS_COUNT;
    objects[OBJECTS_CUBE].lights = lights;
  }

  while(engine->is_running) {
    { // update
      objects[OBJECTS_CUBE].position.y = cos(engine->time_current)*0.2 + 0.5;

      lights[LIGHTS_POINT_0].position.x = sin(engine->time_current);
      lights[LIGHTS_POINT_1].position.x = cos(engine->time_current);
    }

    { // draw
      lgl_draw    (2, objects);
      lite_engine_end_frame  (engine);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
  }

  lite_engine_free(engine);

  return 0;
}
