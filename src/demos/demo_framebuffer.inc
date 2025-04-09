GLuint shader_framebuffer = 0;
lgl_framebuffer framebuffer;
lgl_framebuffer framebuffer_MSAA;
l_object frame_obj;

void demo_framebuffer_alloc(void) {
  GLuint vertex_shader = lgl_shader_compile(
      "res/shaders/frame_buffer_texture_vertex.glsl", GL_VERTEX_SHADER);
  GLuint fragment_shader = lgl_shader_compile(
      "res/shaders/frame_buffer_default_fragment.glsl", GL_FRAGMENT_SHADER);
  shader_framebuffer = lgl_shader_link(vertex_shader, fragment_shader);

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
  lgl_active_framebuffer_set_MSAA(&framebuffer_MSAA);
}
