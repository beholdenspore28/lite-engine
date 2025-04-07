void demo_lines(void) {

  graphics_context->camera.position.z = -2;

  // --------------------------------------------------------------------------
  // Create lines

  l_object lines = l_object_alloc(1);
  lgl_batch lines_batch = lgl_batch_alloc(LGL_ARCHETYPE_EMPTY);

  list_vector3 line_points = list_vector3_alloc();
  // REMEMBER! line rendering is not indexed. therefor each line
  // needs a start and end point.
  list_vector3_add(&line_points, (vector3){  0,  0,  0 });
  list_vector3_add(&line_points, (vector3){  1,  1,  0 });
  list_vector3_add(&line_points, (vector3){  1,  1,  0 });
  list_vector3_add(&line_points, (vector3){  2,  1,  1 });
  list_vector3_add(&line_points, (vector3){  2,  1,  1 });
  list_vector3_add(&line_points, (vector3){  2,  5,  1 });

  lgl_lines_alloc(&lines_batch, line_points);
  list_vector3_free(&line_points);

  lines_batch.render_flags |= LGL_FLAG_DRAW_POINTS;
  lines_batch.shader = shader_solid;
  lines_batch.color = (vector4){1.0, 0.3, 0.3, 1.0};
  lines_batch.lights = &light;
  lines_batch.lights_count = 1;
  lines.transform.scale[0] = vector3_one(1);
  lines.transform.position[0] = (vector3){0, 0, 0};

  // --------------------------------------------------------------------------
  // update
  while (!glfwWindowShouldClose(graphics_context->GLFWwindow)) {
    lgl_update_window_title();

    lgl_camera_update();
    camera_fly(graphics_context);

    lines.transform.position[0].y = 0.2 * sinf(graphics_context->time_current);
    lines.transform.rotation[0] = quaternion_rotate_euler(
        lines.transform.rotation[0], vector3_one(graphics_context->time_delta));

    { // draw scene to the frame
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      lgl_draw(lines, lines_batch);
    }

    lgl_end_frame();
  }

  l_object_free(lines);
  lgl_batch_free(lines_batch);
}
