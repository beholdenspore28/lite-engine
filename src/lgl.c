#include "lgl.h"
#include "blib/blib_file.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <assert.h>

static lgl_framebuffer *lgl__active_framebuffer = NULL;
static lgl_framebuffer *lgl__active_framebuffer_MSAA = NULL;
static lgl_context *lgl__active_context = NULL;

void lgl_active_framebuffer_set(lgl_framebuffer *frame) {
  lgl__active_framebuffer = frame;
}

void lgl_active_framebuffer_set_MSAA(lgl_framebuffer *frame) {
  lgl__active_framebuffer_MSAA = frame;
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

void lgl__buffer_element_array(GLuint *VAO, GLuint *VBO, GLuint *EBO,
                               GLuint vertex_count, lgl_vertex *vertices,
                               GLuint indices_count, GLuint *indices) {
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);

  glGenBuffers(1, VBO);
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);

  glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(lgl_vertex), vertices,
               GL_STATIC_DRAW);

  glGenBuffers(1, EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * indices_count,
               indices, GL_STATIC_DRAW);

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

void lgl_transform_matrix(const lgl_transform *transform) {
    { // Model

      GLfloat model_matrix[16];
      lgl_mat4_identity(model_matrix);

      {
        GLfloat scale[16];
        lgl_mat4_identity(scale);
        scale[0] = transform->scale.x;
        scale[5] = transform->scale.y;
        scale[10] = transform->scale.z;

        GLfloat translation[16];
        lgl_mat4_identity(translation);
        translation[12] = transform->position.x;
        translation[13] = transform->position.y;
        translation[14] = transform->position.z;

        GLfloat rotation[16] = {0};
        quaternion_to_mat4(transform->rotation, rotation);

        lgl_mat4_multiply(model_matrix, scale, rotation);
        lgl_mat4_multiply(model_matrix, model_matrix, translation);
      }
    }
}

void lgl_camera_update(lgl_transform transform) {
  int width, height;
  glfwGetFramebufferSize(lgl__active_context->GLFWwindow, &width, &height);

  const float aspect = (float)width / height;

  GLfloat projection[16];
  lgl_mat4_identity(projection);
  lgl_perspective(projection, 70 * (3.14159 / 180.0), aspect, 0.0001, 1000);

  vector3 offset = vector3_rotate(vector3_back(1.0), transform.rotation);

  GLfloat translation_matrix[16];
  lgl_mat4_identity(translation_matrix);
  translation_matrix[12] = -transform.position.x + offset.x;
  translation_matrix[13] = -transform.position.y + offset.y;
  translation_matrix[14] = -transform.position.z + offset.z;

  GLfloat rotation_matrix[16];
  lgl_mat4_identity(rotation_matrix);
  quaternion_to_mat4(quaternion_conjugate(transform.rotation), rotation_matrix);

  lgl_mat4_multiply(transform.matrix, translation_matrix, rotation_matrix);
  lgl_mat4_multiply(lgl__active_context->camera_matrix, transform.matrix, projection);
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

void lgl_draw_instanced(const lgl_batch batch) {

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

  assert(0); // call lgl_transform_matrix(); here

  {
    GLint model_matrix_location =
      glGetUniformLocation(batch.shader, "u_model_matrix");
    glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, batch.transform.matrix);
  }

  assert(0); // calculate camera matrix
  
  lgl__uniform_materials(batch);
  lgl__uniform_lights(batch);

  glUniform4f(glGetUniformLocation(batch.shader, "u_color"), batch.color.x,
              batch.color.y, batch.color.z, batch.color.w);

  glBindVertexArray(batch.VAO);

  if (batch.render_flags & LGL_FLAG_DRAW_POINTS) {
    glDrawArraysInstanced(
        GL_POINTS, 0, sc_list_lgl_vertex_count(batch.vertices), batch.count);
  }

  switch (batch.primitive) {
  case LGL_PRIMITIVE_LINES: {
    glDrawArraysInstanced(GL_LINES, 0, sc_list_lgl_vertex_count(batch.vertices),
                          batch.count);
  } break;

  case LGL_PRIMITIVE_POINTS: {
    glDrawArraysInstanced(
        GL_POINTS, 0, sc_list_lgl_vertex_count(batch.vertices), batch.count);
  } break;

  case LGL_PRIMITIVE_TRIANGLES_INDEXED: {
    glDrawElementsInstanced(GL_TRIANGLES, sc_list_GLuint_count(batch.indices),
                            GL_UNSIGNED_INT, 0, batch.count);
  } break;

  case LGL_PRIMITIVE_TRIANGLES: {
    glDrawArraysInstanced(GL_TRIANGLES, 0,
                          sc_list_lgl_vertex_count(batch.vertices),
                          batch.count);
  } break;
  }

  glUseProgram(0);
}

void lgl_draw(const lgl_batch *batch) {

  { // render flags
    if ((batch->render_flags & LGL_FLAG_ENABLED) == 0) {
      return;
    }

    if (batch->render_flags & LGL_FLAG_USE_WIREFRAME) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (batch->render_flags & LGL_FLAG_USE_STENCIL) {
      glStencilMask(0xFF);
    } else {
      glStencilMask(0x00);
    }
  }

  glUseProgram(batch->shader);
  glUniform1i(glGetUniformLocation(batch->shader, "u_use_instancing"), 0);

  lgl_transform_matrix(&batch->transform);            

  {
    GLint model_matrix_location =
      glGetUniformLocation(batch->shader, "u_model_matrix");
    glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, batch->transform.matrix);
  }

  assert(0); // calculate camera matrix

  lgl__uniform_materials(*batch);
  lgl__uniform_lights(*batch);

  glBindVertexArray(batch->VAO);

  if (batch->render_flags & LGL_FLAG_DRAW_POINTS) {
    glDrawArrays(
        GL_POINTS, 0,
        sc_list_lgl_vertex_count(batch->vertices)); // for debugging geometry
  }

  switch (batch->primitive) {
    case LGL_PRIMITIVE_LINES: {
                                glDrawArrays(GL_LINES, 0, sc_list_lgl_vertex_count(batch->vertices));
                              } break;

    case LGL_PRIMITIVE_POINTS: {
                                 glDrawArrays(GL_POINTS, 0, sc_list_lgl_vertex_count(batch->vertices));
                               } break;

    case LGL_PRIMITIVE_TRIANGLES_INDEXED: {
                                            glDrawElements(GL_TRIANGLES, sc_list_GLuint_count(batch->indices),
                                                GL_UNSIGNED_INT, 0);
                                          } break;

    case LGL_PRIMITIVE_TRIANGLES: {
                                    glDrawArrays(GL_TRIANGLES, 0, sc_list_lgl_vertex_count(batch->vertices));
                                  } break;
  }

  glUseProgram(0);
}

lgl_batch lgl_batch_alloc(const unsigned int count, const unsigned int archetype) {

  lgl_batch batch = {0};
  batch.count = count;

  batch.transform = (lgl_transform){
    .position={0},
    .rotation={0},
    .scale={0},
    .matrix = calloc(sizeof(GLfloat), 16),
  };

  glGenBuffers(1, &batch.model_matrix_buffer);

  batch.texture_offset = vector2_zero();
  batch.texture_scale = vector2_one(1.0);
  batch.render_flags = LGL_FLAG_ENABLED;
  batch.primitive = LGL_PRIMITIVE_TRIANGLES;

  switch (archetype) {
  case LGL_ARCHETYPE_QUAD: {

    lgl_vertex vertices[6] = {
        // position    //normal              //tex coord
        (lgl_vertex){{-1, -1, 0.0}, vector3_forward(1.0), {0.0, 0.0}},
        (lgl_vertex){{1, -1, 0.0}, vector3_forward(1.0), {1.0, 0.0}},
        (lgl_vertex){{1, 1, 0.0}, vector3_forward(1.0), {1.0, 1.0}},

        (lgl_vertex){{-1, 1, 0.0}, vector3_forward(1.0), {0.0, 1.0}},
        (lgl_vertex){{-1, -1, 0.0}, vector3_forward(1.0), {0.0, 0.0}},
        (lgl_vertex){{1, 1, 0.0}, vector3_forward(1.0), {1.0, 1.0}},
    };

    batch.vertices = sc_list_lgl_vertex_alloc_from_array(vertices, 6);

    lgl__buffer_vertex_array(&batch.VAO, &batch.VBO,
                             sc_list_lgl_vertex_count(batch.vertices),
                             batch.vertices);
  } break;

  case LGL_ARCHETYPE_CUBE: {
    lgl_vertex vertices[36] = {
        (lgl_vertex){{-0.5, -0.5, -0.5}, vector3_back(1.0), {0.0, 0.0}},
        (lgl_vertex){{0.5, -0.5, -0.5}, vector3_back(1.0), {1.0, 0.0}},
        (lgl_vertex){{0.5, 0.5, -0.5}, vector3_back(1.0), {1.0, 1.0}},

        (lgl_vertex){{0.5, 0.5, -0.5}, vector3_back(1.0), {1.0, 1.0}},
        (lgl_vertex){{-0.5, 0.5, -0.5}, vector3_back(1.0), {0.0, 1.0}},
        (lgl_vertex){{-0.5, -0.5, -0.5}, vector3_back(1.0), {0.0, 0.0}},

        (lgl_vertex){{0.5, 0.5, 0.5}, vector3_forward(1.0), {1.0, 1.0}},
        (lgl_vertex){{0.5, -0.5, 0.5}, vector3_forward(1.0), {1.0, 0.0}},
        (lgl_vertex){{-0.5, -0.5, 0.5}, vector3_forward(1.0), {0.0, 0.0}},

        (lgl_vertex){{-0.5, -0.5, 0.5}, vector3_forward(1.0), {0.0, 0.0}},
        (lgl_vertex){{-0.5, 0.5, 0.5}, vector3_forward(1.0), {0.0, 1.0}},
        (lgl_vertex){{0.5, 0.5, 0.5}, vector3_forward(1.0), {1.0, 1.0}},

        (lgl_vertex){{-0.5, -0.5, -0.5}, vector3_left(1.0), {0.0, 1.0}},
        (lgl_vertex){{-0.5, 0.5, -0.5}, vector3_left(1.0), {1.0, 1.0}},
        (lgl_vertex){{-0.5, 0.5, 0.5}, vector3_left(1.0), {1.0, 0.0}},

        (lgl_vertex){{-0.5, 0.5, 0.5}, vector3_left(1.0), {1.0, 0.0}},
        (lgl_vertex){{-0.5, -0.5, 0.5}, vector3_left(1.0), {0.0, 0.0}},
        (lgl_vertex){{-0.5, -0.5, -0.5}, vector3_left(1.0), {0.0, 1.0}},

        (lgl_vertex){{0.5, 0.5, 0.5}, vector3_right(1.0), {1.0, 0.0}},
        (lgl_vertex){{0.5, 0.5, -0.5}, vector3_right(1.0), {1.0, 1.0}},
        (lgl_vertex){{0.5, -0.5, -0.5}, vector3_right(1.0), {0.0, 1.0}},

        (lgl_vertex){{0.5, -0.5, -0.5}, vector3_right(1.0), {0.0, 1.0}},
        (lgl_vertex){{0.5, -0.5, 0.5}, vector3_right(1.0), {0.0, 0.0}},
        (lgl_vertex){{0.5, 0.5, 0.5}, vector3_right(1.0), {1.0, 0.0}},

        (lgl_vertex){{0.5, -0.5, 0.5}, vector3_down(1.0), {1.0, 0.0}},
        (lgl_vertex){{0.5, -0.5, -0.5}, vector3_down(1.0), {1.0, 1.0}},
        (lgl_vertex){{-0.5, -0.5, -0.5}, vector3_down(1.0), {0.0, 1.0}},

        (lgl_vertex){{-0.5, -0.5, -0.5}, vector3_down(1.0), {0.0, 1.0}},
        (lgl_vertex){{-0.5, -0.5, 0.5}, vector3_down(1.0), {0.0, 0.0}},
        (lgl_vertex){{0.5, -0.5, 0.5}, vector3_down(1.0), {1.0, 0.0}},

        (lgl_vertex){{-0.5, 0.5, -0.5}, vector3_up(1.0), {0.0, 1.0}},
        (lgl_vertex){{0.5, 0.5, -0.5}, vector3_up(1.0), {1.0, 1.0}},
        (lgl_vertex){{0.5, 0.5, 0.5}, vector3_up(1.0), {1.0, 0.0}},

        (lgl_vertex){{0.5, 0.5, 0.5}, vector3_up(1.0), {1.0, 0.0}},
        (lgl_vertex){{-0.5, 0.5, 0.5}, vector3_up(1.0), {0.0, 0.0}},
        (lgl_vertex){{-0.5, 0.5, -0.5}, vector3_up(1.0), {0.0, 1.0}},
    };

    batch.vertices = sc_list_lgl_vertex_alloc_from_array(vertices, 36);

    lgl__buffer_vertex_array(&batch.VAO, &batch.VBO,
                             sc_list_lgl_vertex_count(batch.vertices),
                             batch.vertices);
  } break;
  }

  return batch;
}

void lgl_lines_alloc(lgl_batch *batch, sc_list_vector3 points) {
  batch->primitive = LGL_PRIMITIVE_LINES;
  batch->vertices = sc_list_lgl_vertex_alloc();

  for (unsigned int i = 0; i < sc_list_vector3_count(points); i++) {
    lgl_vertex vertex = (lgl_vertex){points[i], vector3_zero(), vector2_zero()};
    sc_list_lgl_vertex_add(&batch->vertices, vertex);
  }

  lgl__buffer_vertex_array(&batch->VAO, &batch->VBO,
                           sc_list_lgl_vertex_count(batch->vertices),
                           batch->vertices);
}

void lgl_icosphere_mesh_alloc(lgl_batch *batch,
                              const unsigned int subdivisions) {

  const float t = (1.0 + sqrt(5.0)) / 2.0;

  lgl_vertex vertices[12] = {
      (lgl_vertex){{-1, t, 0}, {0, 0, 0}, {0, 1}},
      (lgl_vertex){{1, t, 0}, {0, 0, 0}, {0, 1}},
      (lgl_vertex){{-1, -t, 0}, {0, 0, 0}, {0, 1}},
      (lgl_vertex){{1, -t, 0}, {0, 0, 0}, {0, 1}},

      (lgl_vertex){{0, -1, t}, {0, 0, 0}, {0, 1}},
      (lgl_vertex){{0, 1, t}, {0, 0, 0}, {0, 1}},
      (lgl_vertex){{0, -1, -t}, {0, 0, 0}, {0, 1}},
      (lgl_vertex){{0, 1, -t}, {0, 0, 0}, {0, 1}},

      (lgl_vertex){{t, 0, -1}, {0, 0, 0}, {0, 1}},
      (lgl_vertex){{t, 0, 1}, {0, 0, 0}, {0, 1}},
      (lgl_vertex){{-t, 0, -1}, {0, 0, 0}, {0, 1}},
      (lgl_vertex){{-t, 0, 1}, {0, 0, 0}, {0, 1}},
  };

  batch->vertices = sc_list_lgl_vertex_alloc_from_array(vertices, 12);
  batch->indices = sc_list_GLuint_alloc();
  batch->primitive = LGL_PRIMITIVE_TRIANGLES_INDEXED;

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

  for (unsigned int i = 0; i < sc_list_lgl_vertex_count(batch->vertices); i++) {
    batch->vertices[i].position =
        vector3_normalize(batch->vertices[i].position);
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
        const lgl_vertex v1 = batch->vertices[i1];
        const lgl_vertex v2 = batch->vertices[i2];
        const lgl_vertex v3 = batch->vertices[i3];
        // create middle vertices
        const lgl_vertex m1 = (lgl_vertex){
            .position = vector3_lerp(v1.position, v2.position, 0.5)};
        const lgl_vertex m2 = (lgl_vertex){
            .position = vector3_lerp(v2.position, v3.position, 0.5)};
        const lgl_vertex m3 = (lgl_vertex){
            .position = vector3_lerp(v3.position, v1.position, 0.5)};
        sc_list_lgl_vertex_add(&batch->vertices, m1);
        sc_list_lgl_vertex_add(&batch->vertices, m2);
        sc_list_lgl_vertex_add(&batch->vertices, m3);
      }
      { // get new indices
        const unsigned int i4 = sc_list_lgl_vertex_count(batch->vertices) - 3;
        const unsigned int i5 = sc_list_lgl_vertex_count(batch->vertices) - 2;
        const unsigned int i6 = sc_list_lgl_vertex_count(batch->vertices) - 1;

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

    for (unsigned int i = 0; i < sc_list_lgl_vertex_count(batch->vertices);
         i++) {
      batch->vertices[i].position =
          vector3_normalize(batch->vertices[i].position);
      batch->vertices[i].normal = batch->vertices[i].position;
    }
  }

#if 0
  debug_log("final lists ------------------------------------");
  for (unsigned int g = 0; g < sc_list_lgl_vertex_count(batch->vertices); g++) {
    printf("vertex[%3d]", g);
    vector3_print(batch->vertices[g].position, "");
  }

  for (unsigned int g = 0; g < sc_list_GLuint_count(batch->indices); g++) {
    printf("index[%3d] = %3d  ", g, batch->indices[g]);
    if (g % 5 == 0) {
      putchar('\n');
    }
  }
  putchar('\n');
#endif

  lgl__buffer_element_array(
      &batch->VAO, &batch->VBO, &batch->EBO,
      sc_list_lgl_vertex_count(batch->vertices), batch->vertices,
      sc_list_GLuint_count(batch->indices), batch->indices);
}

void lgl_mesh_obj_alloc(lgl_batch *batch, const char *filepath) {
  file_buffer file = file_buffer_alloc(filepath);

  batch->primitive = LGL_PRIMITIVE_TRIANGLES_INDEXED;
  batch->vertices = sc_list_lgl_vertex_alloc();
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
      const int got = sscanf(c, "f %d/%d/%d %d/%d/%d %d/%d/%d",
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
    lgl_vertex v = {0};
    v.position = positions[i];
    sc_list_lgl_vertex_add(&batch->vertices, v);
  }

  sc_list_vector3_free(positions);
  sc_list_vector2_free(texcoords);

  lgl__buffer_element_array(
      &batch->VAO, &batch->VBO, &batch->EBO,
      sc_list_lgl_vertex_count(batch->vertices), batch->vertices,
      sc_list_GLuint_count(batch->indices), batch->indices);
}

void lgl_batch_free(lgl_batch batch) {
  if (batch.vertices) {
    sc_list_lgl_vertex_free(batch.vertices);
  }

  if (batch.indices) {
    sc_list_GLuint_free(batch.indices);
  }

  glDeleteBuffers(1, &batch.model_matrix_buffer);
  glDeleteBuffers(1, &batch.VBO);
  glDeleteBuffers(1, &batch.EBO);
  glDeleteVertexArrays(1, &batch.VAO);
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

  frame.quad = lgl_batch_alloc(1, LGL_ARCHETYPE_QUAD);
  {
    frame.quad.primitive = LGL_PRIMITIVE_TRIANGLES;
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
  lgl__framebuffer_resize(lgl__active_framebuffer_MSAA, width, height);

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

  return lgl__active_context;
}

void lgl_update_window_title(void) {
  static float timer = 0;
  timer += lgl__active_context->time_delta;
  if (timer > 1) { // window titlebar
    timer = 0;
    char window_title[64] = {0};

    snprintf(window_title, sizeof(window_title),
             "Lite-Engine Demo. | %.0lf FPS | %.4f DT",
             lgl__active_context->time_FPS, lgl__active_context->time_delta);

    glfwSetWindowTitle(lgl__active_context->GLFWwindow, window_title);
  }
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
