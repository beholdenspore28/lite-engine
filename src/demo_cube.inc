void demo_cube(void) {

  graphics_context->camera.position.z = -2;

  // --------------------------------------------------------------------------
  // Create cube

  l_object cube = l_object_alloc(1);
  lgl_batch cube_batch = lgl_batch_alloc(L_ARCHETYPE_CUBE);
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

    lgl_update_window_title();

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
