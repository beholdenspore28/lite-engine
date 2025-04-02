#define GLFW_INCLUDE_NONE
#include "lgl.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "blib/blib_file.h"
#include "stb_image.h"

static lgl_framebuffer *lgl__active_framebuffer = NULL;
static lgl_framebuffer *lgl__active_framebuffer_2 = NULL;
static lgl_context *lgl__active_context = NULL;

DEFINE_LIST(lgl_vertex)

void lgl_active_framebuffer_set(lgl_framebuffer *frame) {
  lgl__active_framebuffer = frame;
}

void lgl_active_framebuffer_set_2(lgl_framebuffer *frame) {
  lgl__active_framebuffer_2 = frame;
}

void lgl_perspective(float *mat, const float fov, const float aspect,
                     const float near, const float far) {

  const float cotan = (1.0 / tanf(fov * 0.5));

  mat[0] = (cotan / aspect);
  mat[5] = cotan;
  mat[10] = -((near + far) / (near - far));
  mat[11] = 1.0f;
  mat[14] = ((2.0 * near * far) / (near - far));
}

GLuint lgl_texture_alloc(const char *imageFile) {
  debug_log("Loading texture from '%s'", imageFile);

  /*create texture*/
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  /*set parameters*/
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  /*load texture data from file*/
  int width, height, numChannels;

  stbi_set_flip_vertically_on_load(1);
  unsigned char *data = stbi_load(imageFile, &width, &height, &numChannels, 0);

  /*error check*/
  if (data) {
    if (numChannels == 4) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else if (numChannels == 3) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
  } else {
    debug_error("Failed to load texture from '%s'\n", imageFile);
  }

  /*cleanup*/
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
  return texture;
}

GLuint lgl_shader_compile(const char *file_path, GLenum type) {
  debug_log("compiling shader from '%s'", file_path);
  file_buffer fb = file_buffer_alloc(file_path);
  if (fb.error) { // error check
    debug_error("failed to read shader from '%s'\n", file_path);
  }

  const char *shader_source = fb.text;

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &shader_source, NULL);
  glCompileShader(shader);

  file_buffer_free(fb);

  { // error check
    int success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      debug_error("failed to compile shader : %s%s", file_path, infoLog);
    }
  }

  return shader;
}

GLuint lgl_shader_link(GLuint vertex_shader, GLuint fragment_shader) {
  GLuint shader = glCreateProgram();
  glAttachShader(shader, vertex_shader);
  glAttachShader(shader, fragment_shader);
  glLinkProgram(shader);
  glDetachShader(shader, vertex_shader);
  glDetachShader(shader, fragment_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return shader;
}

void lgl__buffer_vertex_array(GLuint *VAO, GLuint *VBO, GLuint vertex_count,
                              lgl_vertex *vertices) {
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);

  glGenBuffers(1, VBO);
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);

  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(lgl_vertex), vertices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(lgl_vertex),
                        (void *)offsetof(lgl_vertex, position));

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(lgl_vertex),
                        (void *)offsetof(lgl_vertex, normal));

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(lgl_vertex),
                        (void *)offsetof(lgl_vertex, texture_coordinates));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
}

void lgl_mat4_buffer(l_object object, lgl_batch *batch) {

  for (unsigned int i = 0; i < object.count; i++) {

    lgl_mat4_identity(object.transform.matrix + i * 16);

    {
      GLfloat scale[16];
      lgl_mat4_identity(scale);
      scale[0] = object.transform.scale[i].x;
      scale[5] = object.transform.scale[i].y;
      scale[10] = object.transform.scale[i].z;

      GLfloat translation[16];
      lgl_mat4_identity(translation);
      translation[12] = object.transform.position[i].x;
      translation[13] = object.transform.position[i].y;
      translation[14] = object.transform.position[i].z;

      GLfloat rotation[16] = {0};
      quaternion_to_mat4(object.transform.rotation[i], rotation);

      lgl_mat4_multiply(object.transform.matrix + i * 16, scale, rotation);
      lgl_mat4_multiply(object.transform.matrix + i * 16,
                        object.transform.matrix + i * 16, translation);
    }
  }

  // --------------------------------------------------------------------------
  // configure instanced array

  glBindBuffer(GL_ARRAY_BUFFER, batch->model_matrix_buffer);

  glBufferData(GL_ARRAY_BUFFER, object.count * sizeof(GLfloat) * 16,
               &object.transform.matrix[0], GL_STATIC_DRAW);

  glBindVertexArray(batch->VAO);

  // set attribute pointers for matrix (4 times vec4)
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 16,
                        (void *)0);

  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 16,
                        (void *)(1 * sizeof(vector4)));

  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 16,
                        (void *)(2 * sizeof(vector4)));

  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 16,
                        (void *)(3 * sizeof(vector4)));

  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glEnableVertexAttribArray(5);
  glEnableVertexAttribArray(6);

  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
  glVertexAttribDivisor(6, 1);

  glBindVertexArray(0);
}

lgl_camera lgl_camera_alloc(void) {
  lgl_camera camera;
  camera.rotation = quaternion_identity();
  camera.position = vector3_zero();

  camera.view = calloc(sizeof(*camera.view), 16);
  camera.projection = calloc(sizeof(*camera.projection), 16);
  lgl_mat4_identity(camera.view);
  lgl_mat4_identity(camera.projection);

  debug_log("Startup completed successfuly");
  debug_log("Success!");
  return camera;
}

void lgl_camera_free(lgl_camera camera) {
  free(camera.view);
  free(camera.projection);
}

void lgl_camera_update(void) {

  //-----------------------------------------------------------------------
  // PROJECTION

  int width, height;
  glfwGetFramebufferSize(lgl__active_context->GLFWwindow, &width, &height);

  const float aspect = (float)width / height;

  lgl_perspective(lgl__active_context->camera.projection,
                  70 * (3.14159 / 180.0), aspect, 0.001, 1000);

  //-----------------------------------------------------------------------
  // View
  {
    vector3 offset =
        vector3_rotate(vector3_back(1.0), lgl__active_context->camera.rotation);

    GLfloat translation[16];
    lgl_mat4_identity(translation);
    translation[12] = -lgl__active_context->camera.position.x + offset.x;
    translation[13] = -lgl__active_context->camera.position.y + offset.y;
    translation[14] = -lgl__active_context->camera.position.z + offset.z;

    GLfloat rotation[16];
    lgl_mat4_identity(rotation);
    quaternion_to_mat4(
        quaternion_conjugate(lgl__active_context->camera.rotation), rotation);
    lgl_mat4_multiply(lgl__active_context->camera.view, translation, rotation);
  }
}

void lgl__uniform_materials(lgl_batch batch) {
  { // textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, batch.diffuse_map);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, batch.specular_map);

    glUniform2f(glGetUniformLocation(batch.shader, "u_texture_offset"),
                batch.texture_offset.x, batch.texture_offset.y);

    glUniform2f(glGetUniformLocation(batch.shader, "u_texture_scale"),
                batch.texture_scale.x, batch.texture_scale.y);
  }

  { // other material properties
    glUniform1i(glGetUniformLocation(batch.shader, "u_material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(batch.shader, "u_material.specular"), 1);
    glUniform1f(glGetUniformLocation(batch.shader, "u_material.shininess"),
                8.0f);

    glUniform3f(glGetUniformLocation(batch.shader, "u_ambient_light"), 0.2, 0.2,
                0.2);

    glUniform4f(glGetUniformLocation(batch.shader, "u_color"), batch.color.x,
                batch.color.y, batch.color.z, batch.color.w);
  }
}

void lgl__uniform_lights(lgl_batch batch) {

  for (GLuint light = 0; light < batch.lights_count; light++) {
    glUniform1ui(glGetUniformLocation(batch.shader, "u_lights_count"),
                 batch.lights_count);
    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].type", light);

      glUniform1i(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].type);
    }

    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].position",
               light);

      glUniform3f(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].position.x,
                  batch.lights[light].position.y,
                  batch.lights[light].position.z);
    }
    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].direction",
               light);

      glUniform3f(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].direction.x,
                  batch.lights[light].direction.y,
                  batch.lights[light].direction.z);
    }
    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].cut_off",
               light);

      glUniform1f(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].cut_off);
    }
    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].outer_cut_off",
               light);

      glUniform1f(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].outer_cut_off);
    }
    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].constant",
               light);

      glUniform1f(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].constant);
    }
    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].linear",
               light);

      glUniform1f(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].linear);
    }
    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].quadratic",
               light);

      glUniform1f(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].quadratic);
    }
    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].diffuse",
               light);

      glUniform3f(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].diffuse.x, batch.lights[light].diffuse.y,
                  batch.lights[light].diffuse.z);
    }
    {
      char uniform_name[64] = {0};

      snprintf(uniform_name, sizeof(uniform_name), "u_lights[%d].specular",
               light);

      glUniform3f(glGetUniformLocation(batch.shader, uniform_name),
                  batch.lights[light].specular.x,
                  batch.lights[light].specular.y,
                  batch.lights[light].specular.z);
    }
  }
}

void lgl_draw_instanced(l_object object, const lgl_batch batch) {

  { // render flags
    if ((batch.render_flags & LGL_FLAG_ENABLED) == 0) {
      return;
    }

    if (batch.render_flags & LGL_FLAG_USE_WIREFRAME) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (batch.render_flags & LGL_FLAG_USE_STENCIL) {
      glStencilMask(0xFF);
    } else {
      glStencilMask(0x00);
    }
  }

  glUseProgram(batch.shader);
  glUniform1i(glGetUniformLocation(batch.shader, "u_use_instancing"), 1);

  { // Model

    GLfloat model_matrix[16];
    lgl_mat4_identity(model_matrix);

    {
      GLfloat translation[16];
      lgl_mat4_identity(translation);
      translation[12] = object.transform.position[0].x;
      translation[13] = object.transform.position[0].y;
      translation[14] = object.transform.position[0].z;

      GLfloat rotation[16];
      lgl_mat4_identity(rotation);
      quaternion_to_mat4(object.transform.rotation[0], rotation);

      lgl_mat4_multiply(model_matrix, rotation, translation);
    }

    GLint model_matrix_location =
        glGetUniformLocation(batch.shader, "u_model_matrix");
    glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, model_matrix);
  }

  { // camera matrix
    GLfloat camera_matrix[16];
    lgl_mat4_identity(camera_matrix);

    lgl_mat4_multiply(camera_matrix, lgl__active_context->camera.view,
                      lgl__active_context->camera.projection);

    GLint camera_matrix_location =
        glGetUniformLocation(batch.shader, "u_camera_matrix");
    glUniformMatrix4fv(camera_matrix_location, 1, GL_FALSE, camera_matrix);
  }

  lgl__uniform_materials(batch);
  lgl__uniform_lights(batch);

  glUniform4f(glGetUniformLocation(batch.shader, "u_color"), batch.color.x,
              batch.color.y, batch.color.z, batch.color.w);

  glBindVertexArray(batch.VAO);
  glDrawArraysInstanced(GL_TRIANGLES, 0, batch.vertices_count, object.count);
  glUseProgram(0);
}

void lgl_draw(l_object object, const lgl_batch batch) {

  for (size_t i = 0; i < object.count; i++) {

    { // render flags
      if ((batch.render_flags & LGL_FLAG_ENABLED) == 0) {
        continue;
      }

      if (batch.render_flags & LGL_FLAG_USE_WIREFRAME) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }

      if (batch.render_flags & LGL_FLAG_USE_STENCIL) {
        glStencilMask(0xFF);
      } else {
        glStencilMask(0x00);
      }
    }

    glUseProgram(batch.shader);
    glUniform1i(glGetUniformLocation(batch.shader, "u_use_instancing"), 0);

    { // Model

      GLfloat model_matrix[16];
      lgl_mat4_identity(model_matrix);

      {
        GLfloat scale[16];
        lgl_mat4_identity(scale);
        scale[0] = object.transform.scale[i].x;
        scale[5] = object.transform.scale[i].y;
        scale[10] = object.transform.scale[i].z;

        GLfloat translation[16];
        lgl_mat4_identity(translation);
        translation[12] = object.transform.position[i].x;
        translation[13] = object.transform.position[i].y;
        translation[14] = object.transform.position[i].z;

        GLfloat rotation[16] = {0};
        quaternion_to_mat4(object.transform.rotation[i], rotation);

        lgl_mat4_multiply(model_matrix, scale, rotation);
        lgl_mat4_multiply(model_matrix, model_matrix, translation);
      }

      GLint model_matrix_location =
          glGetUniformLocation(batch.shader, "u_model_matrix");
      glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, model_matrix);
    }

    { // camera matrix
      GLfloat camera_matrix[16];
      lgl_mat4_identity(camera_matrix);

      lgl_mat4_multiply(camera_matrix, lgl__active_context->camera.view,
                        lgl__active_context->camera.projection);

      GLint camera_matrix_location =
          glGetUniformLocation(batch.shader, "u_camera_matrix");
      glUniformMatrix4fv(camera_matrix_location, 1, GL_FALSE, camera_matrix);
    }

    lgl__uniform_materials(batch);
    lgl__uniform_lights(batch);

    glBindVertexArray(batch.VAO);
    glDrawArrays(GL_POINTS, 0, batch.vertices_count);
    glUseProgram(0);
  }
}

lgl_batch lgl_batch_alloc(unsigned int count, unsigned int archetype) {

  lgl_batch batch = {0};

  glGenBuffers(1, &batch.model_matrix_buffer);

  for (unsigned int j = 0; j < count; j++) {
    batch.texture_offset = vector2_zero();
    batch.texture_scale = vector2_one(1.0);
    batch.render_flags = LGL_FLAG_ENABLED;
  }

  switch (archetype) {
  case L_ARCHETYPE_QUAD: {
    list_lgl_vertex vertices = list_lgl_vertex_alloc();
    // clang-format off
    // position                    //normal                    //tex coord
    list_lgl_vertex_add(&vertices, (lgl_vertex){{-1, -1, 0.0}, vector3_forward(1.0), {0.0, 0.0}});
    list_lgl_vertex_add(&vertices, (lgl_vertex){{ 1, -1, 0.0}, vector3_forward(1.0), {1.0, 0.0}});
    list_lgl_vertex_add(&vertices, (lgl_vertex){{ 1,  1, 0.0}, vector3_forward(1.0), {1.0, 1.0}});

    list_lgl_vertex_add(&vertices, (lgl_vertex){{-1,  1, 0.0}, vector3_forward(1.0), {0.0, 1.0}});
    list_lgl_vertex_add(&vertices, (lgl_vertex){{-1, -1, 0.0}, vector3_forward(1.0), {0.0, 0.0}});
    list_lgl_vertex_add(&vertices, (lgl_vertex){{ 1,  1, 0.0}, vector3_forward(1.0), {1.0, 1.0}});
    batch.vertices = vertices.array;
    batch.vertices_count = vertices.length;

    lgl__buffer_vertex_array(&batch.VAO, &batch.VBO, batch.vertices_count,
                             batch.vertices);
    // clang-format on
  } break;

  case L_ARCHETYPE_CUBE: {
    // clang-format off
    list_lgl_vertex vertices = list_lgl_vertex_alloc();
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5, -0.5},vector3_back(1.0),   {0.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5, -0.5, -0.5},vector3_back(1.0),   {1.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5, -0.5},vector3_back(1.0),   {1.0, 1.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5, -0.5},vector3_back(1.0),   {1.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5,  0.5, -0.5},vector3_back(1.0),   {0.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5, -0.5},vector3_back(1.0),   {0.0, 0.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5,  0.5},vector3_forward(1.0),{1.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5, -0.5,  0.5},vector3_forward(1.0),{1.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5,  0.5},vector3_forward(1.0),{0.0, 0.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5,  0.5},vector3_forward(1.0),{0.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5,  0.5,  0.5},vector3_forward(1.0),{0.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5,  0.5},vector3_forward(1.0),{1.0, 1.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5, -0.5},vector3_left(1.0),   {0.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5,  0.5, -0.5},vector3_left(1.0),   {1.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5,  0.5,  0.5},vector3_left(1.0),   {1.0, 0.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5,  0.5,  0.5},vector3_left(1.0),   {1.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5,  0.5},vector3_left(1.0),   {0.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5, -0.5},vector3_left(1.0),   {0.0, 1.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5,  0.5},vector3_right(1.0),  {1.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5, -0.5},vector3_right(1.0),  {1.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5, -0.5, -0.5},vector3_right(1.0),  {0.0, 1.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5, -0.5, -0.5},vector3_right(1.0),  {0.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5, -0.5,  0.5},vector3_right(1.0),  {0.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5,  0.5},vector3_right(1.0),  {1.0, 0.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5, -0.5,  0.5},vector3_down(1.0),   {1.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5, -0.5, -0.5},vector3_down(1.0),   {1.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5, -0.5},vector3_down(1.0),   {0.0, 1.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5, -0.5},vector3_down(1.0),   {0.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5, -0.5,  0.5},vector3_down(1.0),   {0.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5, -0.5,  0.5},vector3_down(1.0),   {1.0, 0.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5,  0.5, -0.5},vector3_up(1.0),     {0.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5, -0.5},vector3_up(1.0),     {1.0, 1.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5,  0.5},vector3_up(1.0),     {1.0, 0.0}});

    list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0.5,  0.5,  0.5},vector3_up(1.0),     {1.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5,  0.5,  0.5},vector3_up(1.0),     {0.0, 0.0}});
    list_lgl_vertex_add(&vertices,(lgl_vertex){{-0.5,  0.5, -0.5},vector3_up(1.0),     {0.0, 1.0}});
    // clang-format on

    batch.vertices = vertices.array;
    batch.vertices_count = vertices.length;

    lgl__buffer_vertex_array(&batch.VAO, &batch.VBO, batch.vertices_count,
                             batch.vertices);
  } break;

  default: {
    batch.vertices = NULL;
    batch.vertices_count = 0;
  } break;
  }

  return batch;
}

void lgl_icosphere_mesh_alloc(lgl_batch *batch) {

  const float t = (1.0 + sqrt(5.0)) / 2.0;

  // clang-format off
  list_lgl_vertex vertices = list_lgl_vertex_alloc();
  // position       //normal         //tex coord
  list_lgl_vertex_add(&vertices,(lgl_vertex){{-1,  t,  0}, {0, 0, 0}, {0, 0}});
  list_lgl_vertex_add(&vertices,(lgl_vertex){{ 1,  t,  0}, {0, 0, 0}, {0, 0}});
  list_lgl_vertex_add(&vertices,(lgl_vertex){{-1, -t,  0}, {0, 0, 0}, {0, 0}});
  list_lgl_vertex_add(&vertices,(lgl_vertex){{ 1, -t,  0}, {0, 0, 0}, {0, 0}});

  list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0, -1,  t}, {0, 0, 0}, {0, 0}});
  list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0,  1,  t}, {0, 0, 0}, {0, 0}});
  list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0, -1, -t}, {0, 0, 0}, {0, 0}});
  list_lgl_vertex_add(&vertices,(lgl_vertex){{ 0,  1, -t}, {0, 0, 0}, {0, 0}});

  list_lgl_vertex_add(&vertices,(lgl_vertex){{ t,  0, -1}, {0, 0, 0}, {0, 0}});
  list_lgl_vertex_add(&vertices,(lgl_vertex){{ t,  0,  1}, {0, 0, 0}, {0, 0}});
  list_lgl_vertex_add(&vertices,(lgl_vertex){{-t,  0, -1}, {0, 0, 0}, {0, 0}});
  list_lgl_vertex_add(&vertices,(lgl_vertex){{-t,  0,  1}, {0, 0, 0}, {0, 0}});
  // clang-format on

  batch->vertices = vertices.array;
  batch->vertices_count = 12;

  lgl__buffer_vertex_array(&batch->VAO, &batch->VBO, batch->vertices_count,
                           batch->vertices);
}

void lgl_batch_free(lgl_batch batch) {
  free(batch.vertices);
  glDeleteBuffers(1, &batch.model_matrix_buffer);
}

lgl_framebuffer lgl_framebuffer_alloc(GLuint shader, GLuint samples,
                                      GLuint num_color_attachments,
                                      GLuint width, GLuint height) {

  lgl_framebuffer frame;

  frame.color_buffers =
      calloc(sizeof(*frame.color_buffers), num_color_attachments);
  frame.color_buffers_count = num_color_attachments;
  frame.samples = samples;

  glGenFramebuffers(1, &frame.FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, frame.FBO);

  if (samples > 1) { // configure MSAA enabled framebuffer

    glGenTextures(num_color_attachments, frame.color_buffers);

    for (unsigned int i = 0; i < num_color_attachments; i++) {
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frame.color_buffers[i]);
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width,
                              height, GL_TRUE);

      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER,
                      GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S,
                      GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T,
                      GL_CLAMP_TO_EDGE);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                             GL_TEXTURE_2D_MULTISAMPLE, frame.color_buffers[i],
                             0);
    }

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    glGenRenderbuffers(1, &frame.RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, frame.RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
                                     GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, frame.RBO);

  } else { // configure framebuffer without MSAA
    glGenTextures(num_color_attachments, frame.color_buffers);

    for (unsigned int i = 0; i < num_color_attachments; i++) {
      glBindTexture(GL_TEXTURE_2D, frame.color_buffers[i]);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT,
                   NULL);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      // attach texture to framebuffer
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                             GL_TEXTURE_2D, frame.color_buffers[i], 0);
    }

    glGenRenderbuffers(1, &frame.RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, frame.RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, frame.RBO);
  }

  GLuint attachments[num_color_attachments];
  for (unsigned int i = 0; i > num_color_attachments; i++) {
    attachments[i] = GL_COLOR_ATTACHMENT0 + i;
  }

  glDrawBuffers(num_color_attachments, attachments);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    debug_error("Framebuffer is not complete!");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  frame.quad = lgl_batch_alloc(1, L_ARCHETYPE_QUAD);
  {
    frame.quad.shader = shader;
    frame.quad.diffuse_map = frame.color_buffers[0];
    frame.width = width;
    frame.height = height;
    // frame.quad.render_flags |= LGL_FLAG_USE_WIREFRAME;
  }

  return frame;
}

void lgl_framebuffer_free(lgl_framebuffer frame) {
  lgl_batch_free(frame.quad);
  free(frame.color_buffers);
  glDeleteFramebuffers(1, &frame.FBO);
  glDeleteTextures(frame.color_buffers_count, frame.color_buffers);
  glDeleteRenderbuffers(1, &frame.RBO);
}

static void lgl__framebuffer_resize(lgl_framebuffer *frame, unsigned int width,
                                    unsigned int height) {

  GLuint shader = frame->quad.shader;
  GLuint samples = frame->samples;
  GLuint color_buffers_count = frame->color_buffers_count;

  lgl_framebuffer_free(*frame);
  *frame = lgl_framebuffer_alloc(shader, samples, color_buffers_count, width,
                                 height);
}

void lgl__framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;

  glViewport(0, 0, width, height);
  lgl__framebuffer_resize(lgl__active_framebuffer, width, height);
  lgl__framebuffer_resize(lgl__active_framebuffer_2, width, height);

#if 0
  debug_log("WIDTH %d HEIGHT %d FBO %d NUM_COLOR_ATTACHMENTS %d",
      lgl__active_framebuffer->width,
      lgl__active_framebuffer->height,
      lgl__active_framebuffer->FBO,
      lgl__active_framebuffer->color_buffers_count);
#endif
}

void lgl__glfw_error_callback(int error, const char *description) {
  (void)error;
  fprintf(stderr, "Error: %s\n", description);
}

lgl_context *lgl_start(const int width, const int height) {
  debug_log("Rev up those fryers!");

  if (lgl__active_context != NULL) {
    debug_warn("Failed to initialize lgl context. "
               "A context has already been created."
               "Returning NULL!");
    return NULL;
  }

  lgl__active_context = malloc(sizeof(*lgl__active_context));

  lgl__active_context->is_running = 1;
  lgl__active_context->time_current = 0;
  lgl__active_context->frame_current = 0;
  lgl__active_context->time_delta = 0;
  lgl__active_context->time_last = 0;
  lgl__active_context->time_FPS = 0;

  if (!glfwInit()) {
    debug_error("Failed to initialize GLFW!");
  }

  glfwSetErrorCallback(lgl__glfw_error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  // glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

  lgl__active_context->GLFWwindow =
      glfwCreateWindow(width, height, "Game Window", NULL, NULL);
  if (!lgl__active_context->GLFWwindow) {
    debug_error("Failed to create GLFW window");
  }

  glfwDefaultWindowHints();
  glfwMakeContextCurrent(lgl__active_context->GLFWwindow);

  glfwSetFramebufferSizeCallback(lgl__active_context->GLFWwindow,
                                 lgl__framebuffer_size_callback);

  glfwSwapInterval(0); // disable vsync

  { // center the window
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    unsigned int resolution_width = mode->width;
    unsigned int resolution_height = mode->height;
    unsigned int position_x = (-width / 2) + resolution_width / 2;
    unsigned int position_y = (-height / 2) + resolution_height / 2;

    glfwSetWindowPos(lgl__active_context->GLFWwindow, position_x, position_y);
  }
  glfwShowWindow(lgl__active_context->GLFWwindow);

  glfwSetInputMode(lgl__active_context->GLFWwindow, GLFW_CURSOR,
                   GLFW_CURSOR_DISABLED);

  gladLoadGL(glfwGetProcAddress);

  glClearColor(0.3f, 0.4f, 0.5f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);

  return lgl__active_context;
}

void lgl__time_update(void) {
  lgl__active_context->time_current = glfwGetTime();
  lgl__active_context->time_delta =
      lgl__active_context->time_current - lgl__active_context->time_last;
  lgl__active_context->time_last = lgl__active_context->time_current;
  lgl__active_context->time_FPS = 1 / lgl__active_context->time_delta;
  lgl__active_context->frame_current++;

#if 0  // log time
  debug_log( "\n"
      "time_current:   %lf\n"
      "frame_current:  %llu\n"
      "time_delta:     %lf\n"
      "time_last:      %lf\n"
      "time_FPS:       %lf",
      lgl__active_context->time_current,
      lgl__active_context->frame_current,
      lgl__active_context->time_delta,
      lgl__active_context->time_last,
      lgl__active_context->time_FPS);
#endif // log time
}

void lgl_free(lgl_context *context) {
  debug_log("Shutting down...");

  context->is_running = 0;

  free(context);
  debug_log("Shutdown complete");
}

void lgl_end_frame(void) {
  lgl__time_update();
  glfwPollEvents();
  glfwSwapBuffers(lgl__active_context->GLFWwindow);
}
