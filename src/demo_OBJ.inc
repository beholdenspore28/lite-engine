#include "blib/blib_file.h"
#include <ctype.h>

void obj_load(lgl_batch *batch, const char *filepath) {
  file_buffer file = file_buffer_alloc(filepath);

  batch->primitive = LGL_PRIMITIVE_TRIANGLES_INDEXED;

  for (const char *c = file.text; c < file.text + file.length; c++) {

#if 0
    while (isspace(*c)) {
      ++c;
    }
    if ((*c) == '#') {
      while ((*c) != '\n') {
        ++c;
      }
    }
#endif

#if 1
    printf("%c", *c);
#endif

    if ((*c) == 'v') {
      lgl_vertex v = {0};
      sscanf(c, "v%f%f%f", &v.position.x, &v.position.y, &v.position.z);
      list_lgl_vertex_add(&batch->vertices, v);
#if 0
      debug_log("%f, %f, %f", v.position.x, v.position.y, v.position.z);
#endif
    }

    if ((*c) == 'f') {
      unsigned int position_indices[3] = {0, 0, 0};
      unsigned int normal_indices[3] = {0, 0, 0};
      unsigned int texture_indices[3] = {0, 0, 0};

      // clang-format off
      sscanf(c, "f %d/%d/%d %d/%d/%d %d/%d/%d",
        &position_indices[0],
        &normal_indices[0],
        &texture_indices[0],
        &position_indices[1],
        &normal_indices[1],
        &texture_indices[1],
        &position_indices[2],
        &normal_indices[2],
        &texture_indices[2]);
      // clang-format on
      
      for(unsigned int i = 0; i < 3; i++) {
        list_GLuint_add(&batch->indices, position_indices[i]);
        debug_log("position[%d]{ %f %f %f }", position_indices[i],
          batch->vertices.array[position_indices[i]].position.x,
          batch->vertices.array[position_indices[i]].position.y,
          batch->vertices.array[position_indices[i]].position.z);
      }

#if 1
      debug_log("pos:  %d/%d/%d", position_indices[0], position_indices[1],
                position_indices[2]);
      debug_log("norm: %d/%d/%d", normal_indices[0], normal_indices[1],
                normal_indices[2]);
      debug_log("tex:  %d/%d/%d", texture_indices[0], texture_indices[1],
                texture_indices[2]);
#endif
    }
  }

  lgl__buffer_element_array(&batch->VAO, &batch->VBO, &batch->EBO,
                            batch->vertices.length, batch->vertices.array,
                            batch->indices.length, batch->indices.array);
}

void demo_OBJ(void) {

  graphics_context->camera.position.z = -2;

  // --------------------------------------------------------------------------
  // Create cube

  l_object obj = l_object_alloc(1);
  lgl_batch obj_batch = lgl_batch_alloc(LGL_ARCHETYPE_EMPTY);
  obj_batch.shader = shader_solid;
  obj_batch.color = (vector4){1.0, 0.5, 0.5, 1.0};
  obj_batch.render_flags |= LGL_FLAG_USE_WIREFRAME;
  obj_batch.render_flags |= LGL_FLAG_DRAW_POINTS;

  obj_load(&obj_batch, "res/models/test.obj");

  // --------------------------------------------------------------------------
  // update
  while (!glfwWindowShouldClose(graphics_context->GLFWwindow)) {
    lgl_update_window_title();

    camera_fly(graphics_context);
    lgl_camera_update();

#if 0
    obj.transform.position[0].y = 0.2 * sinf(graphics_context->time_current);
    obj.transform.rotation[0] = quaternion_rotate_euler(
        obj.transform.rotation[0], vector3_one(graphics_context->time_delta));
#endif

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
