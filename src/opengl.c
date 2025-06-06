#include "opengl.h"
#include "file.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <assert.h>

static renderer_gl_framebuffer *renderer_gl__active_framebuffer = NULL;
static renderer_gl_framebuffer *renderer_gl__active_framebuffer_MSAA = NULL;
static renderer_gl_context *renderer_gl__active_context = NULL;

void renderer_gl_active_framebuffer_set(renderer_gl_framebuffer *frame) {
  renderer_gl__active_framebuffer = frame;
}

void renderer_gl_active_framebuffer_set_MSAA(renderer_gl_framebuffer *frame) {
  renderer_gl__active_framebuffer_MSAA = frame;
}

void renderer_gl_perspective(GLfloat *mat, const GLfloat fov,
                             const GLfloat aspect, const GLfloat near,
                             const GLfloat far) {

  const GLfloat cotan = (1.0 / tanf(fov * 0.5));

  mat[0] = (cotan / aspect);
  mat[5] = cotan;
  mat[10] = -((near + far) / (near - far));
  mat[11] = 1.0f;
  mat[14] = ((2.0 * near * far) / (near - far));
}

GLuint renderer_gl_texture_alloc(const char *imageFile) {
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

GLuint renderer_gl_shader_compile(const char *file_path, GLenum type) {
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

GLuint renderer_gl_shader_link(GLuint vertex_shader, GLuint fragment_shader) {
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

void renderer_gl__buffer_vertex_array(GLuint *VAO, GLuint *VBO,
                                      GLuint vertex_count,
                                      renderer_gl_vertex *vertices) {
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);

  glGenBuffers(1, VBO);
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);

  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(renderer_gl_vertex),
               vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(renderer_gl_vertex),
                        (void *)offsetof(renderer_gl_vertex, position));

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(renderer_gl_vertex),
                        (void *)offsetof(renderer_gl_vertex, normal));

  glVertexAttribPointer(
      2, 2, GL_FLOAT, GL_FALSE, sizeof(renderer_gl_vertex),
      (void *)offsetof(renderer_gl_vertex, texture_coordinates));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
}

void renderer_gl__buffer_element_array(GLuint *VAO, GLuint *VBO, GLuint *EBO,
                                       GLuint vertex_count,
                                       renderer_gl_vertex *vertices,
                                       GLuint indices_count, GLuint *indices) {
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);

  glGenBuffers(1, VBO);
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);

  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(renderer_gl_vertex),
               vertices, GL_STATIC_DRAW);

  glGenBuffers(1, EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * indices_count,
               indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(renderer_gl_vertex),
                        (void *)offsetof(renderer_gl_vertex, position));

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(renderer_gl_vertex),
                        (void *)offsetof(renderer_gl_vertex, normal));

  glVertexAttribPointer(
      2, 2, GL_FLOAT, GL_FALSE, sizeof(renderer_gl_vertex),
      (void *)offsetof(renderer_gl_vertex, texture_coordinates));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
}

void renderer_gl__buffer_matrices(const renderer_gl_batch *batch) {
  for (unsigned int i = 0; i < batch->count * 16; i += 16) {
    mat4_identity(batch->matrices + i);

    batch->matrices[0 + i] = batch->transform[i / 16].scale.x;
    batch->matrices[5 + i] = batch->transform[i / 16].scale.y;
    batch->matrices[10 + i] = batch->transform[i / 16].scale.z;
    batch->matrices[12 + i] = batch->transform[i / 16].position.x;
    batch->matrices[13 + i] = batch->transform[i / 16].position.y;
    batch->matrices[14 + i] = batch->transform[i / 16].position.z;

    GLfloat rotation[16] = {0};
    quaternion_to_mat4(batch->transform[i / 16].rotation, rotation);
    mat4_multiply(batch->matrices + i, batch->matrices + i, rotation);
  }

  // --------------------------------------------------------------------------
  // configure instanced array

  glBindBuffer(GL_ARRAY_BUFFER, batch->model_matrix_buffer);

  glBufferData(GL_ARRAY_BUFFER, batch->count * sizeof(GLfloat) * 16,
               &batch->matrices[0], GL_STATIC_DRAW);

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

void renderer_gl_transform_matrix(GLfloat *matrix,
                                  const renderer_gl_transform *transform) {
  { // Model

    mat4_identity(matrix);

    {
      GLfloat scale[16];
      mat4_identity(scale);
      scale[0] = transform[0].scale.x;
      scale[5] = transform[0].scale.y;
      scale[10] = transform[0].scale.z;

      GLfloat translation[16];
      mat4_identity(translation);
      translation[12] = transform[0].position.x;
      translation[13] = transform[0].position.y;
      translation[14] = transform[0].position.z;

      GLfloat rotation[16] = {0};
      quaternion_to_mat4(transform[0].rotation, rotation);

      mat4_multiply(matrix, scale, rotation);
      mat4_multiply(matrix, matrix, translation);
    }
  }
}

void renderer_gl_camera_update(GLfloat *matrix,
                               renderer_gl_transform transform) {
  int width, height;
  glfwGetFramebufferSize(renderer_gl__active_context->GLFWwindow, &width,
                         &height);

  const GLfloat aspect = (GLfloat)width / height;

  GLfloat projection[16];
  mat4_identity(projection);
  renderer_gl_perspective(projection, 70 * (3.14159 / 180.0), aspect, 0.0001,
                          1000);

  vector3 offset = vector3_rotate((vector3){0, 0, -1}, transform.rotation);

  GLfloat translation_matrix[16];
  mat4_identity(translation_matrix);
  translation_matrix[12] = -transform.position.x + offset.x;
  translation_matrix[13] = -transform.position.y + offset.y;
  translation_matrix[14] = -transform.position.z + offset.z;

  GLfloat rotation_matrix[16];
  mat4_identity(rotation_matrix);
  quaternion_to_mat4(quaternion_conjugate(transform.rotation), rotation_matrix);

  mat4_multiply(matrix, translation_matrix, rotation_matrix);
  mat4_multiply(renderer_gl__active_context->camera_matrix, matrix, projection);
}

void renderer_gl__uniform_materials(renderer_gl_batch batch) {
  { // textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, batch.diffuse_map);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, batch.specular_map);
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

void renderer_gl__uniform_lights(renderer_gl_batch batch) {

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

void renderer_gl__draw(const renderer_gl_batch *batch) {
  glUniform1i(glGetUniformLocation(batch->shader, "u_use_instancing"), 0);

  renderer_gl_transform_matrix(batch->matrices, batch->transform);

  {
    GLint model_matrix_location =
        glGetUniformLocation(batch->shader, "u_model_matrix");
    glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, batch->matrices);
  }

  glBindVertexArray(batch->VAO);

  if (batch->render_flags & RENDERER_GL_FLAG_DRAW_POINTS) {
    glDrawArrays(GL_POINTS, 0,
                 sc_list_renderer_gl_vertex_count(batch->vertices));
  }

  renderer_gl__active_context->draw_calls++;

  switch (batch->primitive) {
  case RENDERER_GL_PRIMITIVE_LINES: {
    glDrawArrays(GL_LINES, 0,
                 sc_list_renderer_gl_vertex_count(batch->vertices));
  } break;

  case RENDERER_GL_PRIMITIVE_POINTS: {
    glDrawArrays(GL_POINTS, 0,
                 sc_list_renderer_gl_vertex_count(batch->vertices));
  } break;

  case RENDERER_GL_PRIMITIVE_TRIANGLES_INDEXED: {
    glDrawElements(GL_TRIANGLES, sc_list_GLuint_count(batch->indices),
                   GL_UNSIGNED_INT, 0);
  } break;

  case RENDERER_GL_PRIMITIVE_TRIANGLES: {
    glDrawArrays(GL_TRIANGLES, 0,
                 sc_list_renderer_gl_vertex_count(batch->vertices));
  } break;
  }
}

void renderer_gl__draw_instanced(const renderer_gl_batch *batch) {
  glUniform1i(glGetUniformLocation(batch->shader, "u_use_instancing"), 1);

  renderer_gl__buffer_matrices(batch);

  glBindVertexArray(batch->VAO);

  if (batch->render_flags & RENDERER_GL_FLAG_DRAW_POINTS) {
    glDrawArraysInstanced(GL_POINTS, 0,
                          sc_list_renderer_gl_vertex_count(batch->vertices),
                          batch->count);
  }

  renderer_gl__active_context->draw_calls++;

  switch (batch->primitive) {
  case RENDERER_GL_PRIMITIVE_LINES: {
    glDrawArraysInstanced(GL_LINES, 0,
                          sc_list_renderer_gl_vertex_count(batch->vertices),
                          batch->count);
  } break;

  case RENDERER_GL_PRIMITIVE_POINTS: {
    glDrawArraysInstanced(GL_POINTS, 0,
                          sc_list_renderer_gl_vertex_count(batch->vertices),
                          batch->count);
  } break;

  case RENDERER_GL_PRIMITIVE_TRIANGLES_INDEXED: {
    glDrawElementsInstanced(GL_TRIANGLES, sc_list_GLuint_count(batch->indices),
                            GL_UNSIGNED_INT, 0, batch->count);
  } break;

  case RENDERER_GL_PRIMITIVE_TRIANGLES: {
    glDrawArraysInstanced(GL_TRIANGLES, 0,
                          sc_list_renderer_gl_vertex_count(batch->vertices),
                          batch->count);
  } break;
  }
}

void renderer_gl_draw(const renderer_gl_batch *batch) {

  { // render flags
    if ((batch->render_flags & RENDERER_GL_FLAG_ENABLED) == 0) {
      return;
    }

    if (batch->render_flags & RENDERER_GL_FLAG_USE_WIREFRAME) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (batch->render_flags & RENDERER_GL_FLAG_USE_STENCIL) {
      glStencilMask(0xFF);
    } else {
      glStencilMask(0x00);
    }
  }

  glUseProgram(batch->shader);

  renderer_gl__uniform_materials(*batch);
  renderer_gl__uniform_lights(*batch);

  {
    GLint camera_matrix_location =
        glGetUniformLocation(batch->shader, "u_camera_matrix");
    glUniformMatrix4fv(camera_matrix_location, 1, GL_FALSE,
                       renderer_gl__active_context->camera_matrix);
  }

  if (batch->render_flags & RENDERER_GL_FLAG_USE_INSTANCING) {
    renderer_gl__draw_instanced(batch);
  } else {
    renderer_gl__draw(batch);
  }

  glUseProgram(0);
}

renderer_gl_batch renderer_gl_batch_alloc(const unsigned int count,
                                          const unsigned int archetype) {
  assert(count > 0);
  renderer_gl_batch batch = {0};
  batch.count = count;

  if (count > 1) {
    glGenBuffers(1, &batch.model_matrix_buffer);
  }

  batch.transform = calloc(sizeof(*batch.transform), count);
  for (unsigned int i = 0; i < count; i++) {
    renderer_gl_transform t = (renderer_gl_transform){
        .position = (vector3){0, 0, 0},
        .rotation = (vector4){0, 0, 0, 1},
        .scale = (vector3){1, 1, 1},
    };
    batch.transform[i] = t;
  }

  batch.matrices = calloc(sizeof(*batch.matrices) * 16, count),

  batch.render_flags = RENDERER_GL_FLAG_ENABLED;
  batch.primitive = RENDERER_GL_PRIMITIVE_TRIANGLES;

  switch (archetype) {
  case RENDERER_GL_ARCHETYPE_QUAD: {

    renderer_gl_vertex vertices[6] = {
        // position    //normal              //tex coord
        (renderer_gl_vertex){{-1, -1, 0.0}, (vector3){0, 0, 1}, {0.0, 0.0}},
        (renderer_gl_vertex){{1, -1, 0.0}, (vector3){0, 0, 1}, {1.0, 0.0}},
        (renderer_gl_vertex){{1, 1, 0.0}, (vector3){0, 0, 1}, {1.0, 1.0}},

        (renderer_gl_vertex){{-1, 1, 0.0}, (vector3){0, 0, 1}, {0.0, 1.0}},
        (renderer_gl_vertex){{-1, -1, 0.0}, (vector3){0, 0, 1}, {0.0, 0.0}},
        (renderer_gl_vertex){{1, 1, 0.0}, (vector3){0, 0, 1}, {1.0, 1.0}},
    };

    batch.vertices = sc_list_renderer_gl_vertex_alloc_from_array(vertices, 6);

    renderer_gl__buffer_vertex_array(
        &batch.VAO, &batch.VBO,
        sc_list_renderer_gl_vertex_count(batch.vertices), batch.vertices);
  } break;

  case RENDERER_GL_ARCHETYPE_PYRAMID: {

    renderer_gl_vertex vertices[18] = {
        // position                //normal              //tex coord
        (renderer_gl_vertex){{-0.5, -0.5, 0.5}, (vector3){0, 0, 1}, {0.0, 0.0}},
        (renderer_gl_vertex){{0.0, 0.5, 0.0}, (vector3){0, 0, 1}, {0.5, 1.0}},
        (renderer_gl_vertex){{0.5, -0.5, 0.5}, (vector3){0, 0, 1}, {1.0, 0.0}},
        (renderer_gl_vertex){
            {-0.5, -0.5, -0.5}, (vector3){0, 0, 1}, {0.0, 0.0}},
        (renderer_gl_vertex){{0.5, -0.5, -0.5}, (vector3){0, 0, 1}, {1.0, 0.0}},
        (renderer_gl_vertex){{0.0, 0.5, 0.0}, (vector3){0, 0, 1}, {0.5, 1.0}},
        (renderer_gl_vertex){{-0.5, -0.5, 0.5}, (vector3){0, 0, 1}, {0.0, 0.0}},
        (renderer_gl_vertex){
            {-0.5, -0.5, -0.5}, (vector3){0, 0, 1}, {1.0, 0.0}},
        (renderer_gl_vertex){{0.0, 0.5, 0.0}, (vector3){0, 0, 1}, {0.5, 1.0}},
        (renderer_gl_vertex){{0.5, -0.5, 0.5}, (vector3){0, 0, 1}, {1.0, 0.0}},
        (renderer_gl_vertex){{0.0, 0.5, 0.0}, (vector3){0, 0, 1}, {0.5, 1.0}},
        (renderer_gl_vertex){{0.5, -0.5, -0.5}, (vector3){0, 0, 1}, {0.0, 0.0}},
        (renderer_gl_vertex){
            {-0.5, -0.5, -0.5}, (vector3){0, 0, 1}, {0.0, 0.0}},
        (renderer_gl_vertex){{-0.5, -0.5, 0.5}, (vector3){0, 0, 1}, {0.0, 0.5}},
        (renderer_gl_vertex){{0.5, -0.5, -0.5}, (vector3){0, 0, 1}, {1.0, 0.0}},
        (renderer_gl_vertex){{0.5, -0.5, -0.5}, (vector3){0, 0, 1}, {0.5, 0.0}},
        (renderer_gl_vertex){{-0.5, -0.5, 0.5}, (vector3){0, 0, 1}, {0.0, 0.5}},
        (renderer_gl_vertex){{0.5, -0.5, 0.5}, (vector3){0, 0, 1}, {0.5, 0.5}},
    };

    batch.vertices = sc_list_renderer_gl_vertex_alloc_from_array(vertices, 18);

    renderer_gl__buffer_vertex_array(
        &batch.VAO, &batch.VBO,
        sc_list_renderer_gl_vertex_count(batch.vertices), batch.vertices);
  } break;

  case RENDERER_GL_ARCHETYPE_CUBE: {
    renderer_gl_vertex vertices[36] = {
        (renderer_gl_vertex){
            {-0.5, -0.5, -0.5}, (vector3){0, 0, -1}, {0.0, 0.0}},
        (renderer_gl_vertex){
            {0.5, -0.5, -0.5}, (vector3){0, 0, -1}, {1.0, 0.0}},
        (renderer_gl_vertex){{0.5, 0.5, -0.5}, (vector3){0, 0, -1}, {1.0, 1.0}},

        (renderer_gl_vertex){{0.5, 0.5, -0.5}, (vector3){0, 0, -1}, {1.0, 1.0}},
        (renderer_gl_vertex){
            {-0.5, 0.5, -0.5}, (vector3){0, 0, -1}, {0.0, 1.0}},
        (renderer_gl_vertex){
            {-0.5, -0.5, -0.5}, (vector3){0, 0, -1}, {0.0, 0.0}},

        (renderer_gl_vertex){{0.5, 0.5, 0.5}, (vector3){0, 0, 1}, {1.0, 1.0}},
        (renderer_gl_vertex){{0.5, -0.5, 0.5}, (vector3){0, 0, -1}, {1.0, 0.0}},
        (renderer_gl_vertex){
            {-0.5, -0.5, 0.5}, (vector3){0, 0, -1}, {0.0, 0.0}},

        (renderer_gl_vertex){
            {-0.5, -0.5, 0.5}, (vector3){0, 0, -1}, {0.0, 0.0}},
        (renderer_gl_vertex){{-0.5, 0.5, 0.5}, (vector3){0, 0, -1}, {0.0, 1.0}},
        (renderer_gl_vertex){{0.5, 0.5, 0.5}, (vector3){0, 0, -1}, {1.0, 1.0}},

        (renderer_gl_vertex){
            {-0.5, -0.5, -0.5}, (vector3){-1, 0, 0}, {0.0, 1.0}},
        (renderer_gl_vertex){
            {-0.5, 0.5, -0.5}, (vector3){-1, 0, 0}, {1.0, 1.0}},
        (renderer_gl_vertex){{-0.5, 0.5, 0.5}, (vector3){-1, 0, 0}, {1.0, 0.0}},

        (renderer_gl_vertex){{-0.5, 0.5, 0.5}, (vector3){-1, 0, 0}, {1.0, 0.0}},
        (renderer_gl_vertex){
            {-0.5, -0.5, 0.5}, (vector3){-1, 0, 0}, {0.0, 0.0}},
        (renderer_gl_vertex){
            {-0.5, -0.5, -0.5}, (vector3){-1, 0, 0}, {0.0, 1.0}},

        (renderer_gl_vertex){{0.5, 0.5, 0.5}, (vector3){1, 0, 0}, {1.0, 0.0}},
        (renderer_gl_vertex){{0.5, 0.5, -0.5}, (vector3){1, 0, 0}, {1.0, 1.0}},
        (renderer_gl_vertex){{0.5, -0.5, -0.5}, (vector3){1, 0, 0}, {0.0, 1.0}},

        (renderer_gl_vertex){{0.5, -0.5, -0.5}, (vector3){1, 0, 0}, {0.0, 1.0}},
        (renderer_gl_vertex){{0.5, -0.5, 0.5}, (vector3){1, 0, 0}, {0.0, 0.0}},
        (renderer_gl_vertex){{0.5, 0.5, 0.5}, (vector3){1, 0, 0}, {1.0, 0.0}},

        (renderer_gl_vertex){{0.5, -0.5, 0.5}, (vector3){0, -1, 0}, {1.0, 0.0}},
        (renderer_gl_vertex){
            {0.5, -0.5, -0.5}, (vector3){0, -1, 0}, {1.0, 1.0}},
        (renderer_gl_vertex){
            {-0.5, -0.5, -0.5}, (vector3){0, -1, 0}, {0.0, 1.0}},

        (renderer_gl_vertex){
            {-0.5, -0.5, -0.5}, (vector3){0, -1, 0}, {0.0, 1.0}},
        (renderer_gl_vertex){
            {-0.5, -0.5, 0.5}, (vector3){0, -1, 0}, {0.0, 0.0}},
        (renderer_gl_vertex){{0.5, -0.5, 0.5}, (vector3){0, -1, 0}, {1.0, 0.0}},

        (renderer_gl_vertex){{-0.5, 0.5, -0.5}, (vector3){0, 1, 0}, {0.0, 1.0}},
        (renderer_gl_vertex){{0.5, 0.5, -0.5}, (vector3){0, 1, 0}, {1.0, 1.0}},
        (renderer_gl_vertex){{0.5, 0.5, 0.5}, (vector3){0, 1, 0}, {1.0, 0.0}},

        (renderer_gl_vertex){{0.5, 0.5, 0.5}, (vector3){0, 1, 0}, {1.0, 0.0}},
        (renderer_gl_vertex){{-0.5, 0.5, 0.5}, (vector3){0, 1, 0}, {0.0, 0.0}},
        (renderer_gl_vertex){{-0.5, 0.5, -0.5}, (vector3){0, 1, 0}, {0.0, 1.0}},
    };

    batch.vertices = sc_list_renderer_gl_vertex_alloc_from_array(vertices, 36);

    renderer_gl__buffer_vertex_array(
        &batch.VAO, &batch.VBO,
        sc_list_renderer_gl_vertex_count(batch.vertices), batch.vertices);
  } break;
  }

  return batch;
}

void renderer_gl_lines_alloc(renderer_gl_batch *batch, sc_list_vector3 points) {
  batch->primitive = RENDERER_GL_PRIMITIVE_LINES;
  batch->vertices = sc_list_renderer_gl_vertex_alloc();

  for (unsigned int i = 0; i < sc_list_vector3_count(points); i++) {
    renderer_gl_vertex vertex =
        (renderer_gl_vertex){points[i], (vector3){0, 0, 0}, (vector2){0, 0}};
    sc_list_renderer_gl_vertex_add(&batch->vertices, vertex);
  }

  renderer_gl__buffer_vertex_array(
      &batch->VAO, &batch->VBO,
      sc_list_renderer_gl_vertex_count(batch->vertices), batch->vertices);
}

void renderer_gl_icosphere_mesh_alloc(renderer_gl_batch *batch,
                                      const unsigned int subdivisions) {

  const GLfloat t = (1.0 + sqrt(5.0)) / 2.0;

  renderer_gl_vertex vertices[12] = {
      (renderer_gl_vertex){{-1, t, 0}, {0, 0, 0}, {0, 1}},
      (renderer_gl_vertex){{1, t, 0}, {0, 0, 0}, {0, 1}},
      (renderer_gl_vertex){{-1, -t, 0}, {0, 0, 0}, {0, 1}},
      (renderer_gl_vertex){{1, -t, 0}, {0, 0, 0}, {0, 1}},

      (renderer_gl_vertex){{0, -1, t}, {0, 0, 0}, {0, 1}},
      (renderer_gl_vertex){{0, 1, t}, {0, 0, 0}, {0, 1}},
      (renderer_gl_vertex){{0, -1, -t}, {0, 0, 0}, {0, 1}},
      (renderer_gl_vertex){{0, 1, -t}, {0, 0, 0}, {0, 1}},

      (renderer_gl_vertex){{t, 0, -1}, {0, 0, 0}, {0, 1}},
      (renderer_gl_vertex){{t, 0, 1}, {0, 0, 0}, {0, 1}},
      (renderer_gl_vertex){{-t, 0, -1}, {0, 0, 0}, {0, 1}},
      (renderer_gl_vertex){{-t, 0, 1}, {0, 0, 0}, {0, 1}},
  };

  batch->vertices = sc_list_renderer_gl_vertex_alloc_from_array(vertices, 12);
  batch->indices = sc_list_GLuint_alloc();
  batch->primitive = RENDERER_GL_PRIMITIVE_TRIANGLES_INDEXED;

  {
    enum { indices_count = 60 };
    const GLuint indices[indices_count] = {
        5, 11, 0,  1,  5, 0, 7,  1, 0,  10, 7, 0,  11, 10, 0, 9, 5, 1, 4, 11,
        5, 2,  10, 11, 6, 7, 10, 8, 1,  7,  4, 9,  3,  2,  4, 3, 6, 2, 3, 8,
        6, 3,  9,  8,  3, 5, 9,  4, 11, 4,  2, 10, 2,  6,  7, 6, 8, 1, 8, 9,
    };

    for (unsigned int i = 0; i < indices_count; i++) {
      sc_list_GLuint_add(&batch->indices, indices[i]);
    }
  }

  for (unsigned int i = 0;
       i < sc_list_renderer_gl_vertex_count(batch->vertices); i++) {
    vector3_normalize(&batch->vertices[i].position);
    batch->vertices[i].normal = batch->vertices[i].position;
  }

  // *===============================================*
  // * vertex layout                                 *
  // *===============================================*
  // |                      v1 [i1]                  |
  // |                     /  \                      |
  // |                    /    \                     |
  // |                   /      \                    |
  // |                  /        \                   |
  // |                 /          \                  |
  // |         [i4] m1------------m3 [i6]            |
  // |               /\            /\                |
  // |              /  \          /  \               |
  // |             /    \        /    \              |
  // |            /      \      /      \             |
  // |           /        \    /        \            |
  // |          /          \  /          \           |
  // | [i2] v2 ------------m2-[i5]---- v3 [i3]       |
  // *===============================================*

  for (unsigned int subd = 0; subd < subdivisions; subd++) {
    sc_list_GLuint new_indices = sc_list_GLuint_alloc();
    for (unsigned int tri = 0; tri < sc_list_GLuint_count(batch->indices);
         tri += 3) {
      const unsigned int i1 = batch->indices[tri];
      const unsigned int i2 = batch->indices[tri + 1];
      const unsigned int i3 = batch->indices[tri + 2];
      { // get vertices in this triangle
        const renderer_gl_vertex v1 = batch->vertices[i1];
        const renderer_gl_vertex v2 = batch->vertices[i2];
        const renderer_gl_vertex v3 = batch->vertices[i3];
        // create middle vertices
        const renderer_gl_vertex m1 = (renderer_gl_vertex){
            .position = vector3_lerp(v1.position, v2.position, 0.5)};
        const renderer_gl_vertex m2 = (renderer_gl_vertex){
            .position = vector3_lerp(v2.position, v3.position, 0.5)};
        const renderer_gl_vertex m3 = (renderer_gl_vertex){
            .position = vector3_lerp(v3.position, v1.position, 0.5)};
        sc_list_renderer_gl_vertex_add(&batch->vertices, m1);
        sc_list_renderer_gl_vertex_add(&batch->vertices, m2);
        sc_list_renderer_gl_vertex_add(&batch->vertices, m3);
      }
      { // get new indices
        const unsigned int i4 =
            sc_list_renderer_gl_vertex_count(batch->vertices) - 3;
        const unsigned int i5 =
            sc_list_renderer_gl_vertex_count(batch->vertices) - 2;
        const unsigned int i6 =
            sc_list_renderer_gl_vertex_count(batch->vertices) - 1;

        sc_list_GLuint_add(&new_indices, i4);
        sc_list_GLuint_add(&new_indices, i5);
        sc_list_GLuint_add(&new_indices, i6);

        sc_list_GLuint_add(&new_indices, i1);
        sc_list_GLuint_add(&new_indices, i4);
        sc_list_GLuint_add(&new_indices, i6);

        sc_list_GLuint_add(&new_indices, i4);
        sc_list_GLuint_add(&new_indices, i2);
        sc_list_GLuint_add(&new_indices, i5);

        sc_list_GLuint_add(&new_indices, i6);
        sc_list_GLuint_add(&new_indices, i5);
        sc_list_GLuint_add(&new_indices, i3);
      }
    }
    sc_list_GLuint_free(batch->indices);
    batch->indices = new_indices;

    for (unsigned int i = 0;
         i < sc_list_renderer_gl_vertex_count(batch->vertices); i++) {
      vector3_normalize(&batch->vertices[i].position);
      batch->vertices[i].normal = batch->vertices[i].position;
    }
  }

#if 0
  debug_log("final lists ------------------------------------");
  for (unsigned int g = 0; g < sc_list_renderer_gl_vertex_count(batch->vertices); g++) {
    printf("vertex[%3d]", g);
    printf(vector2_TO_STRING(batch->vertices[g].position));
  }

  for (unsigned int g = 0; g < sc_list_GLuint_count(batch->indices); g++) {
    printf("index[%3d] = %3d  ", g, batch->indices[g]);
    if (g % 5 == 0) {
      putchar('\n');
    }
  }
  putchar('\n');
#endif

  renderer_gl__buffer_element_array(
      &batch->VAO, &batch->VBO, &batch->EBO,
      sc_list_renderer_gl_vertex_count(batch->vertices), batch->vertices,
      sc_list_GLuint_count(batch->indices), batch->indices);
}

void renderer_gl_mesh_obj_alloc(renderer_gl_batch *batch,
                                const char *filepath) {
  file_buffer file = file_buffer_alloc(filepath);

  batch->primitive = RENDERER_GL_PRIMITIVE_TRIANGLES_INDEXED;
  batch->vertices = sc_list_renderer_gl_vertex_alloc();
  batch->indices = sc_list_GLuint_alloc();

  sc_list_vector3 positions = sc_list_vector3_alloc();
  sc_list_vector2 texcoords = sc_list_vector2_alloc();

  for (const char *c = file.text; c < file.text + file.length; c++) {
    // printf("%c", *c);
    if ((*c) == 'v') {
      if (c[1] == 't') {
        vector2 t = {0};
        sscanf(c, "vt%f%f", &t.x, &t.y);
        // debug_log("vt[%lu] { %f %f }", sc_list_vector2_count(texcoords), t.x,
        // t.y);
        sc_list_vector2_add(&texcoords, t);
      } else if (c[1] == 'n') {
      } else {
        vector3 v = {0};
        sscanf(c, "v%f%f%f", &v.x, &v.y, &v.z);
        // debug_log("v[%lu] { %f %f %f }", sc_list_vector3_count(positions),
        // v.x, v.y, v.z);
        sc_list_vector3_add(&positions, v);
      }
    }

    if ((*c) == 'f') {
      unsigned int position_indices[3] = {0, 0, 0};
      unsigned int normal_indices[3] = {0, 0, 0};
      unsigned int texture_indices[3] = {0, 0, 0};

      // clang-format off
      const int got = sscanf(c, "f %u/%u/%u %u/%u/%u %u/%u/%u",
        &position_indices[2],
        &normal_indices[2],
        &texture_indices[2],
        &position_indices[1],
        &normal_indices[1],
        &texture_indices[1],
        &position_indices[0],
        &normal_indices[0],
        &texture_indices[0]);
      // clang-format on

      if (got != 9) {
        debug_error("OOPS!");
      }

      for (unsigned int i = 0; i < 3; i++) {
        sc_list_GLuint_add(&batch->indices, position_indices[i] - 1);
      }
    }
  }

  file_buffer_free(file);

  for (unsigned int i = 0; i < sc_list_vector3_count(positions); i++) {
    renderer_gl_vertex v = {0};
    v.position = positions[i];
    sc_list_renderer_gl_vertex_add(&batch->vertices, v);
  }

  sc_list_vector3_free(positions);
  sc_list_vector2_free(texcoords);

  renderer_gl__buffer_element_array(
      &batch->VAO, &batch->VBO, &batch->EBO,
      sc_list_renderer_gl_vertex_count(batch->vertices), batch->vertices,
      sc_list_GLuint_count(batch->indices), batch->indices);
}

void renderer_gl_batch_free(renderer_gl_batch batch) {
  if (batch.vertices) {
    sc_list_renderer_gl_vertex_free(batch.vertices);
  }

  if (batch.indices) {
    sc_list_GLuint_free(batch.indices);
  }

  free(batch.matrices);
  free(batch.transform);
  glDeleteBuffers(1, &batch.model_matrix_buffer);
  glDeleteBuffers(1, &batch.VBO);
  glDeleteBuffers(1, &batch.EBO);
  glDeleteVertexArrays(1, &batch.VAO);
}

renderer_gl_framebuffer
renderer_gl_framebuffer_alloc(GLuint shader, GLuint samples,
                              GLuint num_color_attachments, GLuint width,
                              GLuint height) {

  renderer_gl_framebuffer frame;

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

  frame.quad = renderer_gl_batch_alloc(1, RENDERER_GL_ARCHETYPE_QUAD);
  {
    frame.quad.primitive = RENDERER_GL_PRIMITIVE_TRIANGLES;
    frame.quad.shader = shader;
    frame.quad.diffuse_map = frame.color_buffers[0];
    frame.width = width;
    frame.height = height;
    // frame.quad.render_flags |= RENDERER_GL_FLAG_USE_WIREFRAME;
  }

  return frame;
}

void renderer_gl_framebuffer_free(renderer_gl_framebuffer frame) {
  renderer_gl_batch_free(frame.quad);
  free(frame.color_buffers);
  glDeleteFramebuffers(1, &frame.FBO);
  glDeleteTextures(frame.color_buffers_count, frame.color_buffers);
  glDeleteRenderbuffers(1, &frame.RBO);
}

static void renderer_gl__framebuffer_resize(renderer_gl_framebuffer *frame,
                                            unsigned int width,
                                            unsigned int height) {

  GLuint shader = frame->quad.shader;
  GLuint samples = frame->samples;
  GLuint color_buffers_count = frame->color_buffers_count;

  renderer_gl_framebuffer_free(*frame);
  *frame = renderer_gl_framebuffer_alloc(shader, samples, color_buffers_count,
                                         width, height);
}

void renderer_gl__framebuffer_size_callback(GLFWwindow *window, int width,
                                            int height) {
  (void)window;

  glViewport(0, 0, width, height);
  if (renderer_gl__active_framebuffer)
    renderer_gl__framebuffer_resize(renderer_gl__active_framebuffer, width,
                                    height);
  if (renderer_gl__active_framebuffer_MSAA)
    renderer_gl__framebuffer_resize(renderer_gl__active_framebuffer_MSAA, width,
                                    height);

#if 0
  debug_log("WIDTH %d HEIGHT %d FBO %d NUM_COLOR_ATTACHMENTS %d",
      renderer_gl__active_framebuffer->width,
      renderer_gl__active_framebuffer->height,
      renderer_gl__active_framebuffer->FBO,
      renderer_gl__active_framebuffer->color_buffers_count);
#endif
}

void renderer_gl__glfw_error_callback(int error, const char *description) {
  (void)error;
  fprintf(stderr, "Error: %s\n", description);
}

renderer_gl_context *renderer_gl_start(const int width, const int height) {
  debug_log("Rev up those fryers!");

  if (renderer_gl__active_context != NULL) {
    debug_warn("Failed to initialize lgl context. "
               "A context has already been created."
               "Returning NULL!");
    return NULL;
  }

  renderer_gl__active_context = malloc(sizeof(*renderer_gl__active_context));

  renderer_gl__active_context->is_running = 1;
  renderer_gl__active_context->time_current = 0;
  renderer_gl__active_context->frame_current = 0;
  renderer_gl__active_context->time_delta = 0;
  renderer_gl__active_context->time_last = 0;
  renderer_gl__active_context->time_FPS = 0;
  renderer_gl__active_context->draw_calls = 0;

  if (!glfwInit()) {
    debug_error("Failed to initialize GLFW!");
  }

  glfwSetErrorCallback(renderer_gl__glfw_error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  // glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

  renderer_gl__active_context->GLFWwindow =
      glfwCreateWindow(width, height, "Game Window", NULL, NULL);
  if (!renderer_gl__active_context->GLFWwindow) {
    debug_error("Failed to create GLFW window");
  }

  glfwDefaultWindowHints();
  glfwMakeContextCurrent(renderer_gl__active_context->GLFWwindow);

  glfwSetFramebufferSizeCallback(renderer_gl__active_context->GLFWwindow,
                                 renderer_gl__framebuffer_size_callback);

  glfwSwapInterval(0); // vsync

  { // center the window
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    unsigned int resolution_width = mode->width;
    unsigned int resolution_height = mode->height;
    unsigned int position_x = (-width / 2) + resolution_width / 2;
    unsigned int position_y = (-height / 2) + resolution_height / 2;

    glfwSetWindowPos(renderer_gl__active_context->GLFWwindow, position_x,
                     position_y);
  }
  glfwShowWindow(renderer_gl__active_context->GLFWwindow);

  gladLoadGL(glfwGetProcAddress);

  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
  glPointSize(5);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);

  debug_log("Startup completed successfuly");
  debug_log("Success!");

  return renderer_gl__active_context;
}

void renderer_gl_update_window_title(void) {
  static GLfloat timer = 0;
  timer += renderer_gl__active_context->time_delta;
  if (timer > 1) { // window titlebar
    timer = 0;
    char window_title[64] = {0};

    snprintf(window_title, sizeof(window_title),
             "Lite-Engine Demo. | %.0lf FPS | %.4f DT | BATCHES %d",
             renderer_gl__active_context->time_FPS,
             renderer_gl__active_context->time_delta,
             renderer_gl__active_context->draw_calls);

    glfwSetWindowTitle(renderer_gl__active_context->GLFWwindow, window_title);
  }
}

void renderer_gl__time_update(void) {
  renderer_gl__active_context->time_current = glfwGetTime();
  renderer_gl__active_context->time_delta =
      renderer_gl__active_context->time_current -
      renderer_gl__active_context->time_last;
  renderer_gl__active_context->time_last =
      renderer_gl__active_context->time_current;
  renderer_gl__active_context->time_FPS =
      1 / renderer_gl__active_context->time_delta;
  renderer_gl__active_context->frame_current++;

#if 0  // log time
  debug_log( "\n"
      "time_current:   %f\n"
      "frame_current:  %llu\n"
      "time_delta:     %f\n"
      "time_last:      %f\n"
      "time_FPS:       %f",
      renderer_gl__active_context->time_current,
      renderer_gl__active_context->frame_current,
      renderer_gl__active_context->time_delta,
      renderer_gl__active_context->time_last,
      renderer_gl__active_context->time_FPS);
#endif // log time
}

void renderer_gl_free(renderer_gl_context *context) {
  debug_log("Shutting down...");

  context->is_running = 0;
  free(context);

  debug_log("Shutdown complete");
}

void renderer_gl_end_frame(void) {
  renderer_gl__time_update();
  glfwPollEvents();
  glfwSwapBuffers(renderer_gl__active_context->GLFWwindow);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  renderer_gl_update_window_title();
  renderer_gl__active_context->draw_calls = 0;
}
