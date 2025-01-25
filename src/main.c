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

  GLuint shader_phong = 0; {
    GLuint vertex_shader   = lgl_shader_compile("res/shaders/phong_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile("res/shaders/phong_fragment.glsl", GL_FRAGMENT_SHADER);
    shader_phong           = lgl_shader_link(vertex_shader, fragment_shader);
  }

  GLuint shader_frame = 0; {
    GLuint vertex_shader   = lgl_shader_compile("res/shaders/frame_buffer_texture_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile("res/shaders/frame_buffer_texture_fragment.glsl", GL_FRAGMENT_SHADER);
    shader_frame           = lgl_shader_link(vertex_shader, fragment_shader);
  }

  GLuint shader_solid = 0; {
    GLuint vertex_shader   = lgl_shader_compile("res/shaders/solid_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile("res/shaders/solid_fragment.glsl", GL_FRAGMENT_SHADER);
    shader_solid           = lgl_shader_link(vertex_shader, fragment_shader);
  }

  objects[OBJECTS_FLOOR] = lgl_cube_alloc(); {
    objects[OBJECTS_FLOOR].shader        = shader_phong;
    objects[OBJECTS_FLOOR].diffuse_map   = lgl_texture_alloc("res/textures/test.png");
    objects[OBJECTS_FLOOR].specular_map  = lgl_texture_alloc("res/textures/default_specular.png");
    objects[OBJECTS_FLOOR].texture_scale = lgl_2f_one(10.0);
    objects[OBJECTS_FLOOR].position.y    = -1;
    objects[OBJECTS_FLOOR].scale         = (lgl_3f_t) {10, 1, 10};
    objects[OBJECTS_FLOOR].lights_count  = LIGHTS_COUNT;
    objects[OBJECTS_FLOOR].lights        = lights;
  }

  objects[OBJECTS_CUBE] = lgl_cube_alloc(); {
    objects[OBJECTS_CUBE].shader         = shader_phong;
    objects[OBJECTS_CUBE].diffuse_map    = lgl_texture_alloc("res/textures/lite-engine-cube.png");
    objects[OBJECTS_CUBE].position.z     = 1;
    objects[OBJECTS_CUBE].lights_count   = LIGHTS_COUNT;
    objects[OBJECTS_CUBE].lights         = lights;
    objects[OBJECTS_CUBE].render_flags  |= LGL_FLAG_USE_STENCIL;
  }

  GLuint framebuffer,
         framebuffer_color_texture; {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);


    glGenTextures             (1, &framebuffer_color_texture);
    glBindTexture             (GL_TEXTURE_2D, framebuffer_color_texture);
    glTexImage2D              (GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri           (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri           (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, framebuffer_color_texture);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        framebuffer_color_texture,
        0);

    GLuint rbo;
    glGenRenderbuffers        (1, &rbo);
    glBindRenderbuffer        (GL_RENDERBUFFER, rbo);
    glRenderbufferStorage     (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 640, 480);
    glBindRenderbuffer        (GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      debug_error("frame buffer is incomplete"); 
      exit(0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  lgl_frame_t frame = lgl_frame_alloc(); {
    frame.shader      = shader_frame;
    frame.diffuse_map = framebuffer_color_texture;
  }

  while(engine->is_running) {
    { // update
      objects[OBJECTS_CUBE].position.y = cos(engine->time_current)*0.2 + 0.5;

      lights[LIGHTS_POINT_0].position.x = sin(engine->time_current);
      lights[LIGHTS_POINT_0].position.z = cos(engine->time_current);
      lights[LIGHTS_POINT_1].position.x = cos(engine->time_current);
      lights[LIGHTS_POINT_1].position.z = sin(engine->time_current);
    }

    { // draw pass 1
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);  
      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw(OBJECTS_COUNT, objects);
      lgl_outline(1, &objects[OBJECTS_CUBE], shader_solid, 0.01);

      //lite_engine_end_frame(engine);
    }

    { // draw pass 2
      glBindFramebuffer(GL_FRAMEBUFFER, 0);  
      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_frame_draw(&frame);

      lite_engine_end_frame(engine);
    }
  }

  glDeleteFramebuffers(1, &framebuffer);

  lite_engine_free(engine);

  return 0;
}
