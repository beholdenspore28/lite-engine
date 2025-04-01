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

  for (unsigned int i = 0; i < stars.count; i++) {

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

typedef struct {
  vector3_t *position_old;
  float bounce;
  float gravity;
  float friction;
  unsigned int count;
} l_verlet_t;

l_verlet_t l_verlet_alloc(lgl_object_t object) {
  l_verlet_t verlet;
  verlet.position_old = calloc(sizeof(vector3_t), object.count);
  verlet.count = object.count;
  verlet.bounce = 0.4;
  verlet.gravity = -0.01;
  verlet.friction = 0.99;

  return verlet;
}

void l_verlet_free(l_verlet_t verlet) { free(verlet.position_old); }

void l_verlet_update(lgl_object_t object, l_verlet_t points) {

  for (unsigned int i = 0; i < points.count; i++) {

    vector3_t velocity =
        vector3_subtract(object.position[i], points.position_old[i]);

    velocity = vector3_scale(velocity, points.friction);

    // debug_log("%f %f %f", velocity.x, velocity.y, velocity.z);

    points.position_old[i] = object.position[i];
    object.position[i] = vector3_add(object.position[i], velocity);
    object.position[i].y += points.gravity;
  }
}

void l_verlet_confine(l_verlet_t verlet, lgl_object_t object,
                      vector3_t bounds) {

  for (unsigned int i = 0; i < object.count; i++) {

    vector3_t velocity =
        vector3_subtract(object.position[i], verlet.position_old[i]);

    if (object.position[i].x > bounds.x) {
      object.position[i].x = bounds.x;
      verlet.position_old[i].x =
          object.position[i].x + velocity.x * verlet.bounce;
    }

    if (object.position[i].y > bounds.y) {
      object.position[i].y = bounds.y;
      verlet.position_old[i].y =
          object.position[i].y + velocity.y * verlet.bounce;
    }

    if (object.position[i].z > bounds.z) {
      object.position[i].z = bounds.z;
      verlet.position_old[i].z =
          object.position[i].z + velocity.z * verlet.bounce;
    }

    if (object.position[i].x < -bounds.x) {
      object.position[i].x = -bounds.x;
      verlet.position_old[i].x =
          object.position[i].x + velocity.x * verlet.bounce;
    }

    if (object.position[i].y < -bounds.y) {
      object.position[i].y = -bounds.y;
      verlet.position_old[i].y =
          object.position[i].y + velocity.y * verlet.bounce;
    }

    if (object.position[i].z < -bounds.z) {
      object.position[i].z = -bounds.z;
      verlet.position_old[i].z =
          object.position[i].z + velocity.z * verlet.bounce;
    }
  }
}

void l_verlet_constrain_distance(lgl_object_t object, unsigned int point_a,
                                 unsigned int point_b,
                                 float distance_constraint) {

  vector3_t diff =
      vector3_subtract(object.position[point_b], object.position[point_a]);
  float distance = vector3_magnitude(diff);
  float adjustment = (distance_constraint - distance) / distance * 0.5;
  vector3_t offset = vector3_scale(diff, adjustment);

  object.position[point_a] = vector3_subtract(object.position[point_a], offset);
  object.position[point_b] = vector3_add(object.position[point_b], offset);
}

void wrap_position(lgl_object_t object, lgl_context_t *lgl_context) {
  for (unsigned int i = 0; i < object.count; i++) {

    object.position[i].x =
        wrap(object.position[i].x, lgl_context->camera.position.x - 15,
             lgl_context->camera.position.x + 15);

    object.position[i].y =
        wrap(object.position[i].y, lgl_context->camera.position.y - 15,
             lgl_context->camera.position.y + 15);

    object.position[i].z =
        wrap(object.position[i].z, lgl_context->camera.position.z - 15,
             lgl_context->camera.position.z + 15);
  }
}

int main() {
  alutInit(0, 0);
  lgl_context_t *lgl_context = lgl_start(1000, 800);

  glClearColor(0.0, 0.0, 0.0, 1);

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
      .position = {0.0, 0.0, 0.0},
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

  lgl_object_t particles = lgl_object_alloc(3, LGL_OBJECT_ARCHETYPE_CUBE);
  particles.shader = shader_solid;
  particles.color = (vector4_t){1.0, 1.0, 1.0, 1.0};
  particles.render_flags |= LGL_FLAG_USE_WIREFRAME;

  l_verlet_t particles_verlet = l_verlet_alloc(particles);

  {
    for (unsigned int i = 0; i < particles.count; i++) {
      particles.scale[i] = vector3_one(0.1);
    }

    particles.position[0] = (vector3_t){0.0, 0.1, 0.0};
    particles.position[1] = (vector3_t){0.1, 0.0, 0.0};
    particles.position[2] = (vector3_t){0.0, 0.0, 0.1};

    vector3_t offset = vector3_up(0.1);
    particles_verlet.position_old[0] =
        vector3_add(particles.position[0], offset);
    particles_verlet.position_old[1] =
        vector3_add(particles.position[1], offset);
    particles_verlet.position_old[2] =
        vector3_add(particles.position[2], offset);
  }

#if 0 // galaxy particle system
  float radius = 5;
  float swirl_strength = 0.1;
  float arm_thickness = 5;
  float arm_length = 10;

  galaxy_generate(particles, radius,
      particles.count * lgl_context->time_current,
      swirl_strength, arm_thickness, arm_length);
#endif

  lgl_mat4_buffer(&particles);

  // --------------------------------------------------------------------------
  // Create cube

  lgl_object_t cube = lgl_object_alloc(1, LGL_OBJECT_ARCHETYPE_CUBE);
  cube.diffuse_map = lgl_texture_alloc("res/textures/lite-engine-cube.png");
  cube.lights = lights;
  cube.lights_count = LIGHTS_COUNT;
  cube.shader = shader_solid;
  cube.color = (vector4_t){1, 1, 1, 1};
  cube.scale[0] = vector3_one(20);
  cube.position[0] = (vector3_t){0, 0, 0};
  cube.render_flags |= LGL_FLAG_USE_WIREFRAME;

  // --------------------------------------------------------------------------
  // Create audio source

  lal_audio_source_t audio_source = lal_audio_source_alloc(1);

  // --------------------------------------------------------------------------
  // game loop

  float timer = 0;
  float timer_physics = 0;

  while (!glfwWindowShouldClose(lgl_context->GLFWwindow)) {

    timer += lgl_context->time_delta;
    timer_physics += lgl_context->time_delta;

    if (timer > 1) { // window titlebar
      timer = 0;
      char window_title[64] = {0};

      snprintf(window_title, sizeof(window_title),
               "Lite-Engine Demo. | %.0lf FPS | %.4f DT", lgl_context->time_FPS,
               lgl_context->time_delta);

      glfwSetWindowTitle(lgl_context->GLFWwindow, window_title);
    }

    camera_update(lgl_context);
    lal_audio_source_update(audio_source, cube, lgl_context);

    if (timer_physics > 0.02) { // update state
      timer_physics = 0;

      // wrap_position(particles, lgl_context);
      l_verlet_confine(particles_verlet, particles, cube.scale[0]);
      l_verlet_update(particles, particles_verlet);
      l_verlet_constrain_distance(particles, 0, 1, 2);
      l_verlet_constrain_distance(particles, 1, 2, 2);
      l_verlet_constrain_distance(particles, 2, 0, 2);

      // cube.rotation[0] = quaternion_rotate_euler(cube.rotation[0],
      // vector3_up(lgl_context->time_delta));

#if 0 // speen (rotate particles around 0,0,0)
      quaternion_t rotation = quaternion_from_euler(vector3_up(0.2 * lgl_context->time_delta));
      for(unsigned int i = 0; i < particles.count; i++) {
        particles.position[i] = vector3_rotate(particles.position[i], rotation);
      }
#endif

      lgl_mat4_buffer(&particles);

      // update lights
      lights[LIGHTS_POINT_0].position = lgl_context->camera.position;
    }

    { // draw scene to the frame
      glBindFramebuffer(GL_FRAMEBUFFER, frame_MSAA.FBO);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      glDisable(GL_CULL_FACE); // TODO add cull face render flag
      lgl_draw(cube);
      glEnable(GL_CULL_FACE);

      lgl_draw_instanced(particles);
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

  l_verlet_free(particles_verlet);
  lal_audio_source_free(audio_source);
  lgl_object_free(cube);
  lgl_object_free(particles);
  lgl_framebuffer_free(frame);
  lgl_framebuffer_free(frame_MSAA);
  lgl_free(lgl_context);

  alutExit();

  return 0;
}
