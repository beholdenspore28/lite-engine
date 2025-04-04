GLuint shader_solid = 0;
GLuint shader_phong = 0;

void demo_shaders_load(void) {
  {
    GLuint vertex_shader =
        lgl_shader_compile("res/shaders/solid_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/solid_fragment.glsl", GL_FRAGMENT_SHADER);
    shader_solid = lgl_shader_link(vertex_shader, fragment_shader);
  }

  {
    GLuint vertex_shader =
        lgl_shader_compile("res/shaders/phong_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/phong_fragment.glsl", GL_FRAGMENT_SHADER);
    shader_phong = lgl_shader_link(vertex_shader, fragment_shader);
  }
}
