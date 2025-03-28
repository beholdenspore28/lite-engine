#include "lgl.h"
#define BLIB_IMPLEMENTATION
#include "blib/blib_math3d.h"

#include "lal.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>

void camera_update(lgl_context_t *context) {
  lgl_camera_update();

  { // movement
    vector3_t movement = vector3_zero();
    {
      movement.y += glfwGetKey(context->GLFWwindow, GLFW_KEY_SPACE) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_LEFT_SHIFT);
      movement.x += glfwGetKey(context->GLFWwindow, GLFW_KEY_D) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_A);
      movement.z += glfwGetKey(context->GLFWwindow, GLFW_KEY_W) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_S);
      movement.x += glfwGetKey(context->GLFWwindow, GLFW_KEY_RIGHT) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_LEFT);
      movement.z += glfwGetKey(context->GLFWwindow, GLFW_KEY_UP) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_DOWN);
    }
    float speed =
        glfwGetKey(context->GLFWwindow, GLFW_KEY_LEFT_CONTROL) ? 20 : 5;
    movement = vector3_normalize(movement);
    movement = vector3_scale(movement, context->time_delta * speed);
    movement = vector3_rotate(movement, context->camera.rotation);
    context->camera.position = vector3_add(context->camera.position, movement);
  }

  { // mouse look
    static int firstFrame = 1;
    static float last_x = 0;
    static float last_y = 0;
    static float pitch = 0;
    static float yaw = 0;

    double mouse_x;
    double mouse_y;
    glfwGetCursorPos(context->GLFWwindow, &mouse_x, &mouse_y);

    if (firstFrame) {
      last_x = mouse_x;
      last_y = mouse_y;
      firstFrame = 0;
    }

    float diff_x = mouse_x - last_x;
    float diff_y = mouse_y - last_y;
    last_x = mouse_x;
    last_y = mouse_y;

    float sensitivity = 0.001;
    diff_x *= sensitivity;
    diff_y *= sensitivity;

    yaw += diff_x;
    pitch += diff_y;

    pitch = clamp(pitch, -PI * 0.5, PI * 0.5);

    quaternion_t rotation =
        quaternion_multiply(quaternion_from_euler(vector3_up(yaw)),
                            quaternion_from_euler(vector3_right(pitch)));
    context->camera.rotation = rotation;
  }
}

void galaxy_generate(lgl_object_t stars, float radius, unsigned int seed,
                     float swirl_strength, float arm_thickness,
                     float arm_length) {

  for (unsigned int i = 0; i < stars.length; i++) {

    // sphere
    stars.position[i] = vector3_point_in_unit_sphere(seed + i);

    // gravity
    vector3_t gravity = vector3_normalize(stars.position[i]);
    stars.position[i] = vector3_subtract(stars.position[i], gravity);

    // stretch
    stars.position[i].x *= arm_thickness;
    stars.position[i].z *= arm_length;

    // swirl
    float swirl_amount =
        vector3_square_magnitude(stars.position[i]) * swirl_strength;

    stars.position[i] = vector3_rotate(
        stars.position[i], quaternion_from_euler(vector3_up(swirl_amount)));

    // scale
    stars.position[i] = vector3_scale(stars.position[i], radius);
  }
}

int main() {

  alutInit(0, 0);
  lgl_context_t *lgl_context = lgl_start(640, 480);

  glClearColor(0, 0, 0, 1);

  // --------------------------------------------------------------------------
  // Create shaders

  GLuint shader_solid = 0;
  {
    GLuint vertex_shader =
        lgl_shader_compile("res/shaders/solid_vertex.glsl", GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/solid_fragment.glsl", GL_FRAGMENT_SHADER);

    shader_solid = lgl_shader_link(vertex_shader, fragment_shader);
  }

  GLuint shader_phong = 0;
  {
    GLuint vertex_shader =
        lgl_shader_compile("res/shaders/phong_vertex.glsl", GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/phong_fragment.glsl", GL_FRAGMENT_SHADER);

    shader_phong = lgl_shader_link(vertex_shader, fragment_shader);
  }

  GLuint shader_framebuffer = 0;
  {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_texture_vertex.glsl", GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_default_fragment.glsl", GL_FRAGMENT_SHADER);

    shader_framebuffer = lgl_shader_link(vertex_shader, fragment_shader);
  }

  // --------------------------------------------------------------------------
  // Create lights

  enum {
    LIGHTS_POINT_0,
    LIGHTS_COUNT, // this should ALWAYS be at the end of the enum
  };
  lgl_light_t lights[LIGHTS_COUNT] = {0};

  lights[LIGHTS_POINT_0] = (lgl_light_t){
      .type = 0,
      .position = {0.0, 1.0, -5.0},
      .direction = {0.0, 0.0, 1.0},
      .cut_off = cos(12.5),
      .outer_cut_off = cos(15.0),
      .constant = 1.0f,
      .linear = 0.09f,
      .quadratic = 0.032f,
      .diffuse = (vector3_t){1.0, 1.0, 1.0},
      .specular = vector3_one(0.6),
  };

  // --------------------------------------------------------------------------
  // Create framebuffer

  enum {
    SAMPLES = 4,
    NUM_COLOR_BUFFERS = 2,
  };

  int width, height;
  glfwGetFramebufferSize(lgl_context->GLFWwindow, &width, &height);

  lgl_framebuffer_t frame = lgl_framebuffer_alloc(
      shader_framebuffer, 1, NUM_COLOR_BUFFERS, width, height);
  lgl_framebuffer_t frame_MSAA = lgl_framebuffer_alloc(
      shader_framebuffer, SAMPLES, NUM_COLOR_BUFFERS, width, height);

  lgl_active_framebuffer_set(&frame);
  lgl_active_framebuffer_set_2(&frame_MSAA);

  // --------------------------------------------------------------------------
  // Create camera

  lgl_context->camera.rotation = quaternion_identity();
  lgl_context->camera.position = vector3_zero();
  lgl_context->camera.position.z = -50;

  GLfloat view[16];
  lgl_mat4_identity(view);
  GLfloat projection[16];
  lgl_mat4_identity(projection);

  lgl_context->camera.view = view;
  lgl_context->camera.projection = projection;

  // --------------------------------------------------------------------------
  // Create particles

  lgl_object_t particles = lgl_object_alloc(10000, LGL_OBJECT_ARCHETYPE_CUBE);
  particles.shader = shader_solid;
  particles.color = (vector4_t){1.0, 1.0, 1.0, 1.0};
  // particles.render_flags |= LGL_FLAG_USE_WIREFRAME;

  lgl_object_t particles_blue =
      lgl_object_alloc(10000, LGL_OBJECT_ARCHETYPE_CUBE);
  particles_blue.shader = shader_solid;
  particles_blue.color = (vector4_t){0.5, 0.5, 1.0, 1.0};
  // particles_blue.render_flags |= LGL_FLAG_USE_WIREFRAME;

  for (unsigned int i = 0; i < particles.length; i++) {
    particles_blue.scale[i] = vector3_one(0.02);
    particles.scale[i] = vector3_one(0.02);
  }

  {
    float radius = 5;
    float swirl_strength = 0.1;
    float arm_thickness = 5;
    float arm_length = 10;

    galaxy_generate(particles, radius, 902347, swirl_strength, arm_thickness,
                    arm_length);
    galaxy_generate(particles_blue, radius, 0, swirl_strength, arm_thickness,
                    arm_length);
  }

  for (unsigned int i = 0; i < particles.length; i++) {

    particles.position[i] = vector3_rotate(
        particles.position[i], quaternion_from_euler(vector3_right(-PI / 8)));

    particles_blue.position[i] =
        vector3_rotate(particles_blue.position[i],
                       quaternion_from_euler(vector3_right(-PI / 8)));
  }

  lgl_mat4_buffer(&particles);
  lgl_mat4_buffer(&particles_blue);

  particles.position[0] = vector3_zero();
  particles.rotation[0] = quaternion_identity();
  particles_blue.position[0] = vector3_zero();
  particles_blue.rotation[0] = quaternion_identity();

  // --------------------------------------------------------------------------
  // Create cube

  lgl_object_t cube = lgl_object_alloc(1, LGL_OBJECT_ARCHETYPE_CUBE);
  cube.diffuse_map = lgl_texture_alloc("res/textures/lite-engine-cube.png");
  cube.lights = lights;
  cube.lights_count = LIGHTS_COUNT;
  cube.shader = shader_phong;
  cube.position[0] = lgl_context->camera.position;
  cube.position[0].x += 30;
  cube.position[0].z += 100;
  cube.color = (vector4_t){1, 1, 1, 1};
  // cube.render_flags |= LGL_FLAG_USE_WIREFRAME;

  // --------------------------------------------------------------------------
  // Create audio source

  lal_audio_source_t audio_source = lal_audio_source_alloc(1);

  // --------------------------------------------------------------------------
  // game loop

  float timer = 0;

  while (!glfwWindowShouldClose(lgl_context->GLFWwindow)) {

    timer += lgl_context->time_delta;

    if (timer > 1) { // window titlebar
      timer = 0;
      char window_title[64] = {0};

      snprintf(window_title, sizeof(window_title),
               "Lite-Engine Demo. | %.0lf FPS | %.4f DT", lgl_context->time_FPS,
               lgl_context->time_delta);

      glfwSetWindowTitle(lgl_context->GLFWwindow, window_title);
    }

    { // update state
      lal_audio_source_update(audio_source, cube, lgl_context);
      camera_update(lgl_context);

      particles.rotation[0] =
          quaternion_from_euler(vector3_up(lgl_context->time_current * -0.03));
      particles_blue.rotation[0].x = particles.rotation[0].x;
      particles_blue.rotation[0].y = particles.rotation[0].y;
      particles_blue.rotation[0].z = particles.rotation[0].z;
      particles_blue.rotation[0].w = particles.rotation[0].w;

      lights[LIGHTS_POINT_0].position = lgl_context->camera.position;
    }

    { // draw scene to the frame
      glBindFramebuffer(GL_FRAMEBUFFER, frame_MSAA.FBO);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      lgl_draw(cube);
      lgl_draw_instanced(particles);
      lgl_draw_instanced(particles_blue);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_MSAA.FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame.FBO);
    glBlitFramebuffer(0, 0, frame_MSAA.width, frame_MSAA.height, 0, 0,
                      frame.width, frame.height, GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);

    { // draw the frame to the screen
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      lgl_draw(frame.quad);
    }

    lgl_end_frame();
  }

  lal_audio_source_free(audio_source);
  lgl_object_free(cube);
  lgl_object_free(particles);
  lgl_object_free(particles_blue);
  lgl_framebuffer_free(frame);
  lgl_framebuffer_free(frame_MSAA);
  lgl_free(lgl_context);

  alutExit();

  return 0;
}
