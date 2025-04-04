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

void update_window_title(void) {
  static float timer = 0;
  timer += graphics_context->time_delta;
  if (timer > 1) { // window titlebar
    timer = 0;
    char window_title[64] = {0};

    snprintf(window_title, sizeof(window_title),
             "Lite-Engine Demo. | %.0lf FPS | %.4f DT",
             graphics_context->time_FPS, graphics_context->time_delta);

    glfwSetWindowTitle(graphics_context->GLFWwindow, window_title);
  }
}

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

#if 1
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
#endif
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

void spinning_cube_demo(void) {

  graphics_context->camera.position.z = -2;

  // --------------------------------------------------------------------------
  // Create cube

  l_object cube = l_object_alloc(1);
  lgl_batch cube_batch = lgl_batch_alloc(1, L_ARCHETYPE_CUBE);
  cube_batch.shader = shader_phong;
  cube_batch.diffuse_map =
      lgl_texture_alloc("res/textures/lite-engine-cube.png");
  cube_batch.color = (vector4){0.5, 0.5, 0.5, 1.0};
  cube_batch.lights = &light;
  cube_batch.lights_count = 1;
  cube.transform.scale[0] = vector3_one(1);
  cube.transform.position[0] = (vector3){0, 0, 0};
  // cube_batch.render_flags |= LGL_FLAG_USE_WIREFRAME;

  // cube audio source component
  lal_audio_source cube_audio_source = lal_audio_source_alloc(cube.count);

  // --------------------------------------------------------------------------
  // update
  while (!glfwWindowShouldClose(graphics_context->GLFWwindow)) {

    update_window_title();

    lal_audio_source_update(cube_audio_source, cube, graphics_context);

    lgl_camera_update();
    // camera_update(graphics_context);

    cube.transform.position[0].y = 0.2 * sinf(graphics_context->time_current);
    cube.transform.rotation[0] = quaternion_rotate_euler(
        cube.transform.rotation[0], vector3_one(graphics_context->time_delta));

    { // draw scene to the frame
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      lgl_draw(cube, cube_batch);
    }

    lgl_end_frame();
  }

  l_object_free(cube);
  lal_audio_source_free(cube_audio_source);
  lgl_batch_free(cube_batch);
}

void procedural_icosphere_demo(void) {

  glDisable(GL_CULL_FACE);
  graphics_context->camera.position.z = -3;

  // --------------------------------------------------------------------------
  // Create sphere

  l_object sphere[2];
  lgl_batch sphere_batch[2];
  for (unsigned int i = 0; i < 2; i++) {
    sphere[i] = l_object_alloc(1);
    sphere_batch[i] = lgl_batch_alloc(1, L_ARCHETYPE_EMPTY);
    sphere_batch[i].shader = shader_solid;
    sphere_batch[i].diffuse_map =
        lgl_texture_alloc("res/textures/lite-engine-cube.png");
    sphere_batch[i].lights = &light;
    sphere_batch[i].lights_count = 1;
    sphere_batch[i].color = (vector4){1, 1, 1, 1};
    sphere[i].transform.scale[0] = vector3_one(1);
    sphere_batch[i].render_flags |= LGL_FLAG_DRAW_POINTS;
    sphere_batch[i].render_flags |= LGL_FLAG_USE_WIREFRAME;
  }
  sphere[0].transform.position[0] = (vector3){1, 0, 0};
  sphere[1].transform.position[0] = (vector3){-1, 0, 0};
  lgl_icosphere_mesh_alloc(&sphere_batch[0], 0);
  lgl_icosphere_mesh_alloc(&sphere_batch[1], 2);

  while (!glfwWindowShouldClose(graphics_context->GLFWwindow)) {
    update_window_title();
    lgl_camera_update();

    for (unsigned int i = 0; i < 2; i++) {
      sphere[i].transform.rotation[0] = quaternion_rotate_euler(
          sphere[i].transform.rotation[0],
          vector3_one(0.1 * graphics_context->time_delta));
    }

    { // draw scene to the frame
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      lgl_draw(sphere[0], sphere_batch[0]);
      lgl_draw(sphere[1], sphere_batch[1]);
    }

    lgl_end_frame();
  }

  l_object_free(sphere[0]);
  l_object_free(sphere[1]);
  lgl_batch_free(sphere_batch[0]);
  lgl_batch_free(sphere_batch[1]);
}

void physics_demo(void) {

  // --------------------------------------------------------------------------
  // Create boundary cube

  l_object cube = l_object_alloc(1);
  lgl_batch cube_batch = lgl_batch_alloc(1, L_ARCHETYPE_CUBE);
  cube_batch.shader = shader_solid;
  cube_batch.color = (vector4){1.0, 1.0, 1.0, 1.0};
  cube.transform.scale[0] = vector3_one(20);
  cube_batch.render_flags |= LGL_FLAG_USE_WIREFRAME;

  // --------------------------------------------------------------------------
  // Create particles

  l_object particles = l_object_alloc(250);
  lgl_batch particles_batch =
      lgl_batch_alloc(particles.count, L_ARCHETYPE_EMPTY);
  particles_batch.shader = shader_solid;
  particles_batch.diffuse_map =
      lgl_texture_alloc("res/textures/lite-engine-cube.png");
  particles_batch.lights = &light;
  particles_batch.lights_count = 1;
  particles_batch.color = (vector4){0.8, 0.3, 0.3, 1.0};
  lgl_icosphere_mesh_alloc(&particles_batch, 2);

  l_verlet_body particles_verlet = l_verlet_body_alloc(particles);

  for (unsigned int i = 0; i < particles.count; i++) {
    particles.transform.scale[i] = vector3_one(0.5);
  }

  // explode in random directions from the origin
  for (unsigned int i = 0; i < particles.count; i++) {
    l_verlet_body_accelerate(particles_verlet, i, vector3_random(i, 1.0));
  }

  // --------------------------------------------------------------------------
  // game loop

  float timer_physics = 0;

  while (!glfwWindowShouldClose(graphics_context->GLFWwindow)) {
    update_window_title();
    camera_update(graphics_context);

    timer_physics += graphics_context->time_delta;
    if (timer_physics > 0.03) { // update state
      timer_physics = 0;

      if (glfwGetMouseButton(graphics_context->GLFWwindow,
                             GLFW_MOUSE_BUTTON_1)) {
        for (unsigned int i = 0; i < particles.count; i++) {
          vector3 force = vector3_normalize(particles.transform.position[i]);
          force = vector3_negate(force);
          force = vector3_scale(force, 0.1);
          l_verlet_body_accelerate(particles_verlet, i, force);
        }
      }

      l_verlet_body_update(particles, particles_verlet);

      for (unsigned int j = 0; j < 2; j++) {
        l_verlet_resolve_collisions(particles);
        l_verlet_body_confine(particles, particles_verlet,
                              vector3_scale(cube.transform.scale[0], 0.5));
      }

      lgl_mat4_buffer(particles, &particles_batch);
    }

    { // draw scene to the frame
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_MSAA.FBO);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      glDisable(GL_CULL_FACE);
      lgl_draw(cube, cube_batch);
      glEnable(GL_CULL_FACE);

      lgl_draw_instanced(particles, particles_batch);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_MSAA.FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.FBO);
    glBlitFramebuffer(0, 0, framebuffer_MSAA.width, framebuffer_MSAA.height, 0,
                      0, framebuffer.width, framebuffer.height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    { // draw the frame to the screen
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      lgl_draw(frame_obj, framebuffer.quad);
    }

    lgl_end_frame();
  }

  l_object_free(cube);
  lgl_batch_free(cube_batch);
  l_object_free(particles);
  l_verlet_body_free(particles_verlet);
  lgl_batch_free(particles_batch);
}

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
  // Create framebuffer

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
  lgl_active_framebuffer_set_2(&framebuffer_MSAA);

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
  spinning_cube_demo();
#endif

#if 0
  physics_demo();
#endif

#if 1
  procedural_icosphere_demo();
#endif

  lgl_camera_free(graphics_context->camera);
  l_object_free(frame_obj);
  lgl_framebuffer_free(framebuffer);
  lgl_framebuffer_free(framebuffer_MSAA);

  lgl_free(graphics_context);

  alutExit();

  return 0;
}
