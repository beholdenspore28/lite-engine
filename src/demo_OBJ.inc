void demo_OBJ(void) {

  graphics_context->camera.position.z = -2;

  // --------------------------------------------------------------------------
  // Create cube

  l_object obj = l_object_alloc(1);
  lgl_batch obj_batch = lgl_batch_alloc(LGL_ARCHETYPE_CUBE);
  obj_batch.shader = shader_phong;
  obj_batch.diffuse_map =
      lgl_texture_alloc("res/textures/lite-engine-cube.png");
  obj_batch.color = (vector4){0.5, 0.5, 0.5, 1.0};
  obj_batch.lights = &light;
  obj_batch.lights_count = 1;
  obj.transform.scale[0] = vector3_one(1);
  obj.transform.position[0] = (vector3){0, 0, 0};
  // obj_batch.render_flags |= LGL_FLAG_USE_WIREFRAME;

  // --------------------------------------------------------------------------
  // update
  while (!glfwWindowShouldClose(graphics_context->GLFWwindow)) {
    lgl_update_window_title();

    camera_fly(graphics_context);
    lgl_camera_update();

    obj.transform.position[0].y = 0.2 * sinf(graphics_context->time_current);
    obj.transform.rotation[0] = quaternion_rotate_euler(
        obj.transform.rotation[0], vector3_one(graphics_context->time_delta));

    { // draw scene to the frame
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);

      lgl_draw(obj, obj_batch);
    }

    lgl_end_frame();
  }

  l_object_free(obj);
  lgl_batch_free(obj_batch);
}
