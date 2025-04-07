#include "physics.h"

void demo_physics(void) {

  // --------------------------------------------------------------------------
  // Create boundary cube

  l_object cube = l_object_alloc(1);
  lgl_batch cube_batch = lgl_batch_alloc(LGL_ARCHETYPE_CUBE);
  cube_batch.shader = shader_solid;
  cube_batch.color = (vector4){1.0, 1.0, 1.0, 1.0};
  cube.transform.scale[0] = vector3_one(20);
  cube_batch.render_flags |= LGL_FLAG_USE_WIREFRAME;

  // --------------------------------------------------------------------------
  // Create particles

  l_object particles = l_object_alloc(250);
  lgl_batch particles_batch =
      lgl_batch_alloc(LGL_ARCHETYPE_EMPTY);
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
    lgl_update_window_title();
    camera_fly(graphics_context);

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
