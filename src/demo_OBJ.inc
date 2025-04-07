#include "blib/blib_file.h"
#include <ctype.h>

void obj_load(lgl_batch *batch, const char* filepath) {
  file_buffer file = file_buffer_alloc(filepath);

  batch->primitive = LGL_PRIMITIVE_TRIANGLES;
  
  for(const char *c = file.text; c < file.text+file.length; c++) {

#if 0
    while (isspace(*c)) { ++c; }
    if ((*c) == '#') {
      while((*c)!='\n') { ++c; }
    }
#endif

    printf("%c", *c);

    switch(*c) {
      case 'v': {
        lgl_vertex v = {0};
        sscanf(c, "v%f%f%f", &v.position.x, &v.position.y, &v.position.z);
        list_lgl_vertex_add(&batch->vertices, v);
        debug_log("%f, %f, %f", v.position.x, v.position.y, v.position.z);
      } break;
    }
  }
  lgl__buffer_vertex_array(&batch->VAO, &batch->VBO, batch->vertices.length,
                             batch->vertices.array);
}

void demo_OBJ(void) {

  graphics_context->camera.position.z = -2;

  // --------------------------------------------------------------------------
  // Create cube

  l_object obj = l_object_alloc(1);
  lgl_batch obj_batch = lgl_batch_alloc(LGL_ARCHETYPE_EMPTY);
  obj_batch.shader = shader_solid;
  obj_batch.color = (vector4){1.0, 0.5, 0.5, 1.0};
  // obj_batch.render_flags |= LGL_FLAG_USE_WIREFRAME;
  obj_batch.render_flags |= LGL_FLAG_DRAW_POINTS;

  obj_load(&obj_batch, "res/models/test.obj");

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
