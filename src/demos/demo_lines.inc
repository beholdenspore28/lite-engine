void demo_lines(void) {

  graphics_context->camera.position.z = -2;
  glPointSize(10);
  glLineWidth(5);

  // --------------------------------------------------------------------------
  // Create lines for the x axis

  l_object lines_x = l_object_alloc(1);
  lgl_batch lines_x_batch = lgl_batch_alloc(LGL_ARCHETYPE_EMPTY);

  sc_list_vector3 lines_x_points = sc_list_vector3_alloc();
  sc_list_vector3_add(&lines_x_points, (vector3){  0,  0,  0 });
  sc_list_vector3_add(&lines_x_points, (vector3){  1,  0,  0 });

  lgl_lines_alloc(&lines_x_batch, lines_x_points);
  sc_list_vector3_free(lines_x_points);

  lines_x_batch.render_flags |= LGL_FLAG_DRAW_POINTS;
  lines_x_batch.shader = shader_solid;
  lines_x_batch.color = (vector4){1.0, 0.3, 0.3, 1.0};
  lines_x_batch.lights = &light;
  lines_x_batch.lights_count = 1;
  lines_x.transform.scale[0] = vector3_one(1);

  // --------------------------------------------------------------------------
  // Create lines for the y axis

  l_object lines_y = l_object_alloc(1);
  lgl_batch lines_y_batch = lgl_batch_alloc(LGL_ARCHETYPE_EMPTY);

  sc_list_vector3 lines_y_points = sc_list_vector3_alloc();
  sc_list_vector3_add(&lines_y_points, (vector3){  0,  0,  0 });
  sc_list_vector3_add(&lines_y_points, (vector3){  0,  1,  0 });

  lgl_lines_alloc(&lines_y_batch, lines_y_points);
  sc_list_vector3_free(lines_y_points);

  lines_y_batch.render_flags |= LGL_FLAG_DRAW_POINTS;
  lines_y_batch.shader = shader_solid;
  lines_y_batch.color = (vector4){0.3, 1.0, 0.3, 1.0};
  lines_y_batch.lights = &light;
  lines_y_batch.lights_count = 1;
  lines_y.transform.scale[0] = vector3_one(1);

  // --------------------------------------------------------------------------
  // Create lines for the z axis

  l_object lines_z = l_object_alloc(1);
  lgl_batch lines_z_batch = lgl_batch_alloc(LGL_ARCHETYPE_EMPTY);

  sc_list_vector3 lines_z_points = sc_list_vector3_alloc();
  sc_list_vector3_add(&lines_z_points, (vector3){  0,  0,  0 });
  sc_list_vector3_add(&lines_z_points, (vector3){  0,  0,  1 });

  lgl_lines_alloc(&lines_z_batch, lines_z_points);
  sc_list_vector3_free(lines_z_points);

  lines_z_batch.render_flags |= LGL_FLAG_DRAW_POINTS;
  lines_z_batch.shader = shader_solid;
  lines_z_batch.color = (vector4){0.3, 0.3, 1.0, 1.0};
  lines_z_batch.lights = &light;
  lines_z_batch.lights_count = 1;
  lines_z.transform.scale[0] = vector3_one(1);

  // --------------------------------------------------------------------------
  // update
  while (!glfwWindowShouldClose(graphics_context->GLFWwindow)) {
    lgl_update_window_title();

    camera_fly(graphics_context);
    lgl_camera_update();

    vector3 offset = vector3_rotate((vector3){0,0,5}, graphics_context->camera.rotation);
    lines_x.transform.position[0] = vector3_add(graphics_context->camera.position, offset);
    lines_y.transform.position[0] = vector3_add(graphics_context->camera.position, offset);
    lines_z.transform.position[0] = vector3_add(graphics_context->camera.position, offset);

    { // draw scene to the frame
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      lgl_draw(lines_x, lines_x_batch);
      lgl_draw(lines_y, lines_y_batch);
      lgl_draw(lines_z, lines_z_batch);
    }

    lgl_end_frame();
  }

  l_object_free(lines_x);
  lgl_batch_free(lines_x_batch);
  lgl_batch_free(lines_y_batch);
  lgl_batch_free(lines_z_batch);
}
