typedef struct {
  float radius;
  unsigned int seed;
  float swirl_strength;
  float arm_thickness_x;
  float arm_thickness_y;
  float arm_length;
} galaxy_data;

void galaxy_generate(l_object stars, galaxy_data data) {

  for (unsigned int i = 0; i < stars.count; i++) {

    // sphere
    stars.transform.position[i] = vector3_random(data.seed + i, 1.0);

    // gravity
    vector3 gravity = vector3_normalize(stars.transform.position[i]);
    stars.transform.position[i] =
        vector3_subtract(stars.transform.position[i], gravity);

    // stretch
    stars.transform.position[i].x *= data.arm_thickness_x;
    stars.transform.position[i].y /= data.arm_thickness_y;
    stars.transform.position[i].z *= data.arm_length;

    // swirl
    float swirl_amount = vector3_square_magnitude(stars.transform.position[i]) *
                         data.swirl_strength;

    stars.transform.position[i] =
        vector3_rotate(stars.transform.position[i],
                       quaternion_from_euler(vector3_up(swirl_amount)));

    // scale
    stars.transform.position[i] =
        vector3_scale(stars.transform.position[i], data.radius);
  }
}

void demo_galaxy(void) {

  glClearColor(0, 0, 0, 1);

  // --------------------------------------------------------------------------
  // Create particles

  l_object particles = l_object_alloc(20000);
  lgl_batch particles_batch =
      lgl_batch_alloc(particles.count, L_ARCHETYPE_EMPTY);
  particles_batch.shader = shader_unlit;
  particles_batch.color = (vector4){1.0, 1.0, 1.0, 1.0};
  lgl_icosphere_mesh_alloc(&particles_batch, 1);

  for (unsigned int i = 0; i < particles.count; i++) {
    particles.transform.scale[i] = vector3_one(0);
  }

  lgl_mat4_buffer(particles, &particles_batch);

  // --------------------------------------------------------------------------
  // game loop

  while (!glfwWindowShouldClose(graphics_context->GLFWwindow)) {
    lgl_update_window_title();
    camera_fly(graphics_context);

    { // draw scene to the frame
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_MSAA.FBO);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      glDisable(GL_CULL_FACE);
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

  l_object_free(particles);
  lgl_batch_free(particles_batch);
}
