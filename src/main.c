#include "platform.h"
#include "lgl.h"

int main() {
  lgl_context_t *context = lgl_start();

  GLuint shader_phong = 0; {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/phong_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/phong_fragment.glsl",
        GL_FRAGMENT_SHADER);

    shader_phong = lgl_shader_link(vertex_shader, fragment_shader);
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

  objects[OBJECTS_FLOOR] = lgl_cube_alloc(context); {
    objects[OBJECTS_FLOOR].shader        =  shader_phong;
    objects[OBJECTS_FLOOR].diffuse_map   =  texture_diffuse;
    objects[OBJECTS_FLOOR].specular_map  =  texture_specular;
    objects[OBJECTS_FLOOR].texture_scale =  lgl_2f_one(10.0);
    objects[OBJECTS_FLOOR].position.y    = -2;
    objects[OBJECTS_FLOOR].scale         =  (lgl_3f_t) {5, 0.5, 5};
    objects[OBJECTS_FLOOR].lights_count  =  LIGHTS_COUNT;
    objects[OBJECTS_FLOOR].lights        =  lights;
  }

  objects[OBJECTS_CUBE] = lgl_cube_alloc(context); {
    objects[OBJECTS_CUBE].shader         =  shader_phong;
    objects[OBJECTS_CUBE].diffuse_map    =  texture_cube;
    objects[OBJECTS_CUBE].position.z     =  1;
    objects[OBJECTS_CUBE].lights_count   =  LIGHTS_COUNT;
    objects[OBJECTS_CUBE].lights         =  lights;
    objects[OBJECTS_CUBE].render_flags  |=  LGL_FLAG_USE_STENCIL;
  }

  GLuint shader_framebuffer = 0; {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_texture_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_texture_fragment.glsl",
        GL_FRAGMENT_SHADER);

    shader_framebuffer = lgl_shader_link(vertex_shader, fragment_shader);
  }

  // set up floating point framebuffer to render scene to
  unsigned int hdrFBO;
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  unsigned int colorBuffers[2];
  glGenTextures(2, colorBuffers);
  for (unsigned int i = 0; i < 2; i++)
  {
    glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA16F, 640, 480, 0, GL_RGBA, GL_FLOAT, NULL
        );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // attach texture to framebuffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
        );
  }

  GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, attachments);

  lgl_render_data_t framebuffer_quad = lgl_cube_alloc(context); {
    framebuffer_quad.shader        =  shader_framebuffer;
    framebuffer_quad.diffuse_map   =  colorBuffers[0];
  }

  while(context->is_running) {
    { // update
      objects[OBJECTS_CUBE].position.y = cos(context->time_current)*0.2 + 0.5;
      objects[OBJECTS_CUBE].rotation = lgl_4f_multiply(
          objects[OBJECTS_CUBE].rotation,
          lgl_4f_from_euler((lgl_3f_t) { 0, context->time_delta, 0 }));
      objects[OBJECTS_FLOOR].texture_offset.y += context->time_delta;

      debug_log("TEX OFF x %f", objects[OBJECTS_FLOOR].texture_offset.x);
      debug_log("TEX OFF Y %f", objects[OBJECTS_FLOOR].texture_offset.y);

      lights[LIGHTS_POINT_0].position.x = sin(context->time_current);
      lights[LIGHTS_POINT_0].position.z = cos(context->time_current);
      lights[LIGHTS_POINT_1].position.x = cos(context->time_current);
      lights[LIGHTS_POINT_1].position.z = sin(context->time_current);
    }

    { // draw scene to the frame
      glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

      glClearColor(0.5,0,0.5,1);
      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw(OBJECTS_COUNT, objects);
    }

    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw(1, &framebuffer_quad);
    }

    lgl_end_frame(context);
  }

  lgl_free(context);

  return 0;
}
