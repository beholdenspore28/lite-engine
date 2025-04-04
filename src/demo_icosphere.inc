void demo_icosphere(void) {

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
    lgl_update_window_title();
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
