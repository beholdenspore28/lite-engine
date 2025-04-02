#include "lal.h"
#include "lgl.h"
#include "physics.h"

#define BLIB_IMPLEMENTATION
#include "blib/blib_math3d.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>

void camera_update(lgl_context *context) {
  lgl_camera_update();

  { // movement
    vector3 movement = vector3_zero();
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

    quaternion rotation =
        quaternion_multiply(quaternion_from_euler(vector3_up(yaw)),
                            quaternion_from_euler(vector3_right(pitch)));
    context->camera.rotation = rotation;
  }
}

void galaxy_generate(l_object stars, float radius, unsigned int seed,
                     float swirl_strength, float arm_thickness,
                     float arm_length) {

  for (unsigned int i = 0; i < stars.count; i++) {

    // sphere
    stars.transform.position[i] = vector3_random(seed + i, 1.0);

    // gravity
    vector3 gravity = vector3_normalize(stars.transform.position[i]);
    stars.transform.position[i] =
        vector3_subtract(stars.transform.position[i], gravity);

    // stretch
    stars.transform.position[i].x *= arm_thickness;
    stars.transform.position[i].z *= arm_length;

    // swirl
    float swirl_amount =
        vector3_square_magnitude(stars.transform.position[i]) * swirl_strength;

    stars.transform.position[i] =
        vector3_rotate(stars.transform.position[i],
                       quaternion_from_euler(vector3_up(swirl_amount)));

    // scale
    stars.transform.position[i] =
        vector3_scale(stars.transform.position[i], radius);
  }
}

int main() {
  alutInit(0, 0);
  lgl_context *lgl_context = lgl_start(1000, 800);

  // glClearColor(0.0, 0.0, 0.0, 1);

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
  // Create framebuffer

  enum {
    SAMPLES = 4,
    NUM_COLOR_BUFFERS = 2,
  };

  int width, height;
  glfwGetFramebufferSize(lgl_context->GLFWwindow, &width, &height);

  l_object frame_obj = l_object_alloc(1);

  lgl_framebuffer frame = lgl_framebuffer_alloc(
      shader_framebuffer, 1, NUM_COLOR_BUFFERS, width, height);
  lgl_framebuffer frame_MSAA = lgl_framebuffer_alloc(
      shader_framebuffer, SAMPLES, NUM_COLOR_BUFFERS, width, height);

  lgl_active_framebuffer_set(&frame);
  lgl_active_framebuffer_set_2(&frame_MSAA);

  // --------------------------------------------------------------------------
  // Create lights and camera

  enum {
    LIGHTS_POINT_0,
    LIGHTS_COUNT, // this should ALWAYS be at the end of the enum
  };
  lgl_light lights[LIGHTS_COUNT] = {0};

  lights[LIGHTS_POINT_0] = (lgl_light){
      .type = 0,
      .position = {0.0, 0.0, 0.0},
      .direction = {0.0, 0.0, 1.0},
      .cut_off = cos(12.5),
      .outer_cut_off = cos(15.0),
      .constant = 1.0f,
      .linear = 0.09f,
      .quadratic = 0.032f,
      .diffuse = (vector3){1.0, 1.0, 1.0},
      .specular = vector3_one(0.6),
  };

  lgl_context->camera = lgl_camera_alloc();
  lgl_context->camera.position.z -= 25;

  // --------------------------------------------------------------------------
  // Create cube

  l_object cube = l_object_alloc(1);
  lgl_batch cube_batch = lgl_batch_alloc(1, L_ARCHETYPE_CUBE);
  cube_batch.diffuse_map =
      lgl_texture_alloc("res/textures/lite-engine-cube.png");
  cube_batch.lights = lights;
  cube_batch.lights_count = LIGHTS_COUNT;
  cube_batch.shader = shader_phong;
  cube_batch.color = (vector4){1.0, 1.0, 1.0, 1.0};
  cube.transform.scale[0] = vector3_one(1);
  cube.transform.position[0] = (vector3){0, 0, -15};
  // cube_batch.render_flags |= LGL_FLAG_USE_WIREFRAME;

  lal_audio_source cube_audio_source = lal_audio_source_alloc(1);

  // --------------------------------------------------------------------------
  // Create particles_batch

  l_object particles = l_object_alloc(200);
  lgl_batch particles_batch =
      lgl_batch_alloc(particles.count, L_ARCHETYPE_CUBE);
  particles_batch.diffuse_map =
      lgl_texture_alloc("res/textures/lite-engine-cube.png");
  particles_batch.lights = lights;
  particles_batch.lights_count = LIGHTS_COUNT;
  particles_batch.shader = shader_phong;
  particles_batch.color = (vector4){1.0, 1.0, 1.0, 1.0};
  // particles_batch.render_flags |= LGL_FLAG_USE_WIREFRAME;

  l_verlet_body particles_verlet = l_verlet_body_alloc(particles);

  for (unsigned int i = 0; i < particles.count; i++) {
    particles.transform.scale[i] = vector3_one(0.5);
  }

#if 1 // random points in a box
  for (unsigned int i = 0; i < particles.count; i++) {
    particles.transform.position[i] = vector3_random_box(i, vector3_one(5));
    particles_verlet.position_old[i] = particles.transform.position[i];
  }
#endif

#if 0 // explode in random directions from the origin
  for (unsigned int i = 0; i < particles.count; i++) {
    l_verlet_body_accelerate(particles_verlet, i, vector3_random(i, 1.0));
  }
#endif

  // --------------------------------------------------------------------------
  // game loop

  float timer = 0;
  float timer_physics = 0;

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

    timer_physics += lgl_context->time_delta;
    if (timer_physics > 0.03) { // update state
      timer_physics = 0;

      for (unsigned int i = 0; i < particles.count; i++) {
        l_verlet_body_accelerate(particles_verlet, i, vector3_down(0.01));
      }

      l_verlet_body_update(particles, particles_verlet);

      for (unsigned int j = 0; j < 2; j++) {
        l_verlet_resolve_collisions(particles);
        l_verlet_body_confine(particles, particles_verlet, vector3_one(10));
      }

      lgl_mat4_buffer(particles, &particles_batch);
    }

    // update lights
    lights[LIGHTS_POINT_0].position = lgl_context->camera.position;

    camera_update(lgl_context);
    lal_audio_source_update(cube_audio_source, cube, lgl_context);

#if 0
    cube.transform.rotation[0] =
        quaternion_rotate_euler(cube.transform.rotation[0],
                                vector3_up(PI / 5 * lgl_context->time_delta));
    cube.transform.position[0] = vector3_add(
        cube.transform.position[0],
        l_object_forward(cube.transform, 0, lgl_context->time_delta * 10));
#endif

    { // draw scene to the frame
      glBindFramebuffer(GL_FRAMEBUFFER, frame_MSAA.FBO);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      glDisable(GL_CULL_FACE); // TODO add cull face render flag
      lgl_draw(cube, cube_batch);
      glEnable(GL_CULL_FACE);

      lgl_draw_instanced(particles, particles_batch);
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

      lgl_draw(frame_obj, frame.quad);
    }

    lgl_end_frame();
  }

  l_object_free(particles);
  l_object_free(cube);
  lgl_camera_free(lgl_context->camera);
  l_verlet_body_free(particles_verlet);
  lal_audio_source_free(cube_audio_source);
  lgl_batch_free(cube_batch);
  lgl_batch_free(particles_batch);

  l_object_free(frame_obj);
  lgl_framebuffer_free(frame);
  lgl_framebuffer_free(frame_MSAA);

  lgl_free(lgl_context);

  alutExit();

  return 0;
}
