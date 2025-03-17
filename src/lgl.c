#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "lgl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "blib/blib_file.h"

#include <time.h>

static lgl_framebuffer_t *lgl__active_framebuffer;
static lgl_context_t     *lgl__active_context;

void lgl_active_framebuffer_set(lgl_framebuffer_t* frame) {
  lgl__active_framebuffer = frame;
}

void lgl_active_context_set(lgl_context_t* context) {
  lgl__active_context = context;
}

DEFINE_LIST(lgl_vertex_t)
DEFINE_LIST(GLuint)

static const float
             LGL__LEFT    = -1.0,
             LGL__RIGHT   =  1.0,
             LGL__UP      =  1.0,
             LGL__DOWN    = -1.0,
             LGL__FORWARD =  1.0,
             LGL__BACK    = -1.0;

/*Multiplies a 4x4 matrix with another 4x4 matrix*/
static inline void lgl__mat4_multiply(
    float *result,
    const float *a,
    const float *b) {

  // row 0
  result[ 0] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3];
  result[ 1] = a[ 0] * b[ 1] + a[ 1] * b[ 5] + a[ 2] * b[ 9] + a[ 3] * b[13];
  result[ 2] = a[ 0] * b[ 2] + a[ 1] * b[ 6] + a[ 2] * b[10] + a[ 3] * b[14];
  result[ 3] = a[ 0] * b[ 3] + a[ 1] * b[ 7] + a[ 2] * b[11] + a[ 3] * b[15];

  // row 1
  result[ 4] = a[ 4] * b[ 0] + a[ 5] * b[ 4] + a[ 6] * b[ 8] + a[ 7] * b[12];
  result[ 5] = a[ 4] * b[ 1] + a[ 5] * b[ 5] + a[ 6] * b[ 9] + a[ 7] * b[13];
  result[ 6] = a[ 4] * b[ 2] + a[ 5] * b[ 6] + a[ 6] * b[10] + a[ 7] * b[14];
  result[ 7] = a[ 4] * b[ 3] + a[ 5] * b[ 7] + a[ 6] * b[11] + a[ 7] * b[15];

  // row 2
  result[ 8] = a[ 8] * b[ 0] + a[ 9] * b[ 4] + a[10] * b[ 8] + a[11] * b[12];
  result[ 9] = a[ 8] * b[ 1] + a[ 9] * b[ 5] + a[10] * b[ 9] + a[11] * b[13];
  result[10] = a[ 8] * b[ 2] + a[ 9] * b[ 6] + a[10] * b[10] + a[11] * b[14];
  result[11] = a[ 8] * b[ 3] + a[ 9] * b[ 7] + a[10] * b[11] + a[11] * b[15];

  // row 3
  result[12] = a[12] * b[ 0] + a[13] * b[ 4] + a[14] * b[ 8] + a[15] * b[12];
  result[13] = a[12] * b[ 1] + a[13] * b[ 5] + a[14] * b[ 9] + a[15] * b[13];
  result[14] = a[12] * b[ 2] + a[13] * b[ 6] + a[14] * b[10] + a[15] * b[14];
  result[15] = a[12] * b[ 3] + a[13] * b[ 7] + a[14] * b[11] + a[15] * b[15];
}

void lgl_perspective(
    float *mat,
    const float fov,
    const float aspect,
    const float near,
    const float far) {

  const float cotan = (1.0 / tanf(fov * 0.5));

  mat[ 0]  = (cotan / aspect);
  mat[ 5]  = cotan;
  mat[10]  = -((near + far) / (near - far));
  mat[11]  = 1.0f;
  mat[14]  = ((2.0 * near * far) / (near - far));
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
  int width,
      height,
      numChannels;

  stbi_set_flip_vertically_on_load(1);
  unsigned char *data = stbi_load(imageFile, &width, &height, &numChannels, 0);

  /*error check*/
  if (data) {
    if (numChannels == 4) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
          0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else if (numChannels == 3) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
          GL_RGB, GL_UNSIGNED_BYTE, data);
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

  const char* shader_source = fb.text;

  GLuint shader = glCreateShader(type);
  glShaderSource   (shader, 1, &shader_source, NULL);
  glCompileShader  (shader);

  file_buffer_free(fb);

  { // error check
    int  success;
    char  infoLog[512];

    glGetShaderiv  (shader, GL_COMPILE_STATUS, &success);

    if(!success) {
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      debug_error(
          "failed to compile shader : %s%s", file_path, infoLog);
    }
  }

  return shader;
}

GLuint lgl_shader_link (GLuint vertex_shader, GLuint fragment_shader) {
  GLuint shader = glCreateProgram();
  glAttachShader  (shader, vertex_shader);
  glAttachShader  (shader, fragment_shader);
  glLinkProgram   (shader);
  glDetachShader  (shader, vertex_shader);
  glDetachShader  (shader, fragment_shader);
  glDeleteShader  (vertex_shader);
  glDeleteShader  (fragment_shader);
  return shader;
}

void lgl__buffer_vertex_array (
    GLuint       *VAO,
    GLuint       *VBO,
    GLuint        vertex_count,
    lgl_vertex_t *vertices) {
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);

  glGenBuffers(1, VBO);
  glBindBuffer(GL_ARRAY_BUFFER, *VBO);

  glBufferData(GL_ARRAY_BUFFER, 
      vertex_count*sizeof(lgl_vertex_t), 
      vertices, 
      GL_STATIC_DRAW);

  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, sizeof(lgl_vertex_t),
      (void*)offsetof(lgl_vertex_t, position));

  glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, sizeof(lgl_vertex_t),
      (void*)offsetof(lgl_vertex_t, normal));

  glVertexAttribPointer(
      2, 2, GL_FLOAT, GL_FALSE, sizeof(lgl_vertex_t),
      (void*)offsetof(lgl_vertex_t, texture_coordinates));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
}

void lgl_outline(
    const size_t         data_length,
    lgl_render_data_t   *data,
    const GLuint         outline_shader,
    const float          thickness){
  for(size_t i = 0; i < data_length; i++) { 
    if ((data[i].render_flags & LGL_FLAG_USE_STENCIL) == 0) {
      debug_warn(
          "object[%lu] is not set to use the stencil buffer, "
          "but you are trying to outline it.", i);
    }
    glStencilFunc (GL_NOTEQUAL, 1, 0xFF);
    glStencilMask (0x00);

    GLuint shader_tmp = data[i].shader;

    glUseProgram(outline_shader);

    data[i].shader = outline_shader;

    glUniform4f(glGetUniformLocation(outline_shader, "u_color"),
        0.0, 1.0, 0.5, 1.0);

    vector3_t scale_tmp = data[i].scale;
    data[i].scale.x *= (1+thickness);
    data[i].scale.y *= (1+thickness);
    data[i].scale.z *= (1+thickness);

    lgl_draw(1, &data[i]);

    data[i].scale  = scale_tmp;
    data[i].shader = shader_tmp;
  }
}

void lgl_draw(
    const size_t             data_length,
    const lgl_render_data_t *data) {
  for(size_t i = 0; i < data_length; i++) {

    glUseProgram(data[i].shader);

#if 0 // log render flags
    debug_log(" ");
    printf("FLAGS AT data[%lu] { ", i);
    for(size_t j = 0; j < sizeof(data[i].render_flags)*8; j++) {
      size_t flag = (data[i].render_flags & (1 << j));
      printf("%u ", flag ? 1 : 0 );
    }
    printf("}\n");
#endif

    { // render flags
      if ((data[i].render_flags & LGL_FLAG_ENABLED) == 0) {
        continue;
      }

      if (data[i].render_flags & LGL_FLAG_USE_WIREFRAME) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }

      if (data[i].render_flags & LGL_FLAG_USE_STENCIL) {
        glStencilMask(0xFF);
      } else {
        glStencilMask(0x00);
      }
    }

    { // MVP
      GLfloat projection[16] = {
        1.0,  0.0,  0.0,  0.0,
        0.0,  1.0,  0.0,  0.0,
        0.0,  0.0,  1.0,  0.0,
        0.0,  0.0,  0.0,  1.0,
      };

      int width, height;
      glfwGetFramebufferSize(lgl__active_context->GLFWwindow, &width, &height);

      const float aspect = (float)width / height;

      lgl_perspective(projection, 80 * (3.14159/180.0), aspect, 0.001, 1000);

      GLfloat scale[16] = {
        data[i].scale.x,    0.0,                0.0,                0.0,
        0.0,                data[i].scale.y,    0.0,                0.0,
        0.0,                0.0,                data[i].scale.z,    0.0,
        0.0,                0.0,                0.0,                1.0,
      };

      GLfloat translation[16] = {
        1.0,                0.0,                0.0,                0.0,
        0.0,                1.0,                0.0,                0.0,
        0.0,                0.0,                1.0,                0.0,
        data[i].position.x, data[i].position.y, data[i].position.z, 1.0,
      };

      GLfloat rotation[16] = {0};
      quaternion_to_mat4(data[i].rotation, rotation);

      GLfloat model[16] = {
        1.0,  0.0,  0.0,  0.0,
        0.0,  1.0,  0.0,  0.0,
        0.0,  0.0,  1.0,  0.0,
        0.0,  0.0,  0.0,  1.0,
      };

      lgl__mat4_multiply(model, rotation, translation);
      lgl__mat4_multiply(model, model, scale);

      GLfloat mvp[16] = {
        1.0,  0.0,  0.0,  0.0,
        0.0,  1.0,  0.0,  0.0,
        0.0,  0.0,  1.0,  0.0,
        0.0,  0.0,  0.0,  1.0,
      };

      lgl__mat4_multiply(mvp, model, projection);

      GLint mvp_location = glGetUniformLocation(data[i].shader, "u_mvp");
      glUniformMatrix4fv(mvp_location, 1, GL_FALSE, mvp);
    }

    { // textures
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, data[i].diffuse_map);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, data[i].specular_map);

      glUniform2f(glGetUniformLocation(data[i].shader, "u_texture_offset"),
          data[i].texture_offset.x,
          data[i].texture_offset.y);

      glUniform2f(glGetUniformLocation(data[i].shader, "u_texture_scale"),
          data[i].texture_scale.x,
          data[i].texture_scale.y);
    }

    { // other material properties
      glUniform1i(glGetUniformLocation(data[i].shader, "u_material.diffuse"), 0);
      glUniform1i(glGetUniformLocation(data[i].shader, "u_material.specular"), 1);
      glUniform1f(glGetUniformLocation(data[i].shader, "u_material.shininess"), 8.0f);

      glUniform3f(
          glGetUniformLocation(data[i].shader, "u_ambient_light"), 
          0.2, 0.2, 0.2);
    }

    // lighting uniforms
    for(GLuint light = 0; light < data[i].lights_count; light++) {
      glUniform1ui(glGetUniformLocation(data[i].shader, "u_lights_count"),
          data[i].lights_count);
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].type",
            light);

        glUniform1i(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].type);
      }
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].position",
            light);

        glUniform3f(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].position.x,
            data[i].lights[light].position.y,
            data[i].lights[light].position.z);
      }
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].direction",
            light);
        
        glUniform3f(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].direction.x,
            data[i].lights[light].direction.y,
            data[i].lights[light].direction.z);
      }
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].cut_off",
            light);

        glUniform1f(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].cut_off);
      }
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].outer_cut_off",
            light);

        glUniform1f(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].outer_cut_off);
      }
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].constant",
            light);

        glUniform1f(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].constant);
      }
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].linear",
            light);

        glUniform1f(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].linear);
      }
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].quadratic",
            light);

        glUniform1f(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].quadratic);
      }
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].diffuse",
            light);

        glUniform3f(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].diffuse.x,
            data[i].lights[light].diffuse.y,
            data[i].lights[light].diffuse.z);
      }
      {
        char uniform_name[64] = {0};

        snprintf(
            uniform_name,
            sizeof(uniform_name),
            "u_lights[%d].specular",
            light);
        
        glUniform3f(glGetUniformLocation(data[i].shader, uniform_name),
            data[i].lights[light].specular.x,
            data[i].lights[light].specular.y,
            data[i].lights[light].specular.z);
      }
    }

    glBindVertexArray(data[i].VAO);
    glDrawArrays(GL_TRIANGLES, 0, data[i].vertex_count);
    glUseProgram(0);

  }
}

lgl_render_data_t lgl_quad_alloc() {
  lgl_render_data_t quad = {0};

  enum { quad_vertices_count = 6 };
  lgl_vertex_t quad_vertices[quad_vertices_count] = { 
    //position                        //normal          //tex coord
    { { LGL__LEFT,  LGL__DOWN, 0.0 }, vector3_forward(1.0), { 0.0, 0.0 } },
    { { LGL__RIGHT, LGL__DOWN, 0.0 }, vector3_forward(1.0), { 1.0, 0.0 } },
    { { LGL__RIGHT, LGL__UP,   0.0 }, vector3_forward(1.0), { 1.0, 1.0 } },

    { { LGL__LEFT,  LGL__UP,   0.0 }, vector3_forward(1.0), { 0.0, 1.0 } },
    { { LGL__LEFT,  LGL__DOWN, 0.0 }, vector3_forward(1.0), { 0.0, 0.0 } },
    { { LGL__RIGHT, LGL__UP,   0.0 }, vector3_forward(1.0), { 1.0, 1.0 } },
  };

  quad.vertices        = quad_vertices;
  quad.vertex_count    = quad_vertices_count;

  quad.scale           = vector3_one(1.0);
  quad.position        = vector3_zero();
  quad.rotation        = quaternion_identity();

  quad.texture_offset  = vector2_zero();
  quad.texture_scale   = vector2_one(1.0);

  quad.render_flags    = LGL_FLAG_ENABLED;

  lgl__buffer_vertex_array(
      &quad.VAO,
      &quad.VBO,
      quad.vertex_count,
      quad.vertices);

  return quad;
}

lgl_render_data_t lgl_cube_alloc() {
  lgl_render_data_t cube = {0};

  enum { cube_vertices_count = 36 };
  lgl_vertex_t cube_vertices[cube_vertices_count] = { 
    //position                                 //normal             //tex coord
    { { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, vector3_back(1.0),    { 0.0, 0.0 } },
    { { LGL__RIGHT, LGL__DOWN, LGL__BACK    }, vector3_back(1.0),    { 1.0, 0.0 } },
    { { LGL__RIGHT, LGL__UP,   LGL__BACK    }, vector3_back(1.0),    { 1.0, 1.0 } },

    { { LGL__RIGHT, LGL__UP,   LGL__BACK    }, vector3_back(1.0),    { 1.0, 1.0 } },
    { { LGL__LEFT,  LGL__UP,   LGL__BACK    }, vector3_back(1.0),    { 0.0, 1.0 } },
    { { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, vector3_back(1.0),    { 0.0, 0.0 } },

    { { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, vector3_forward(1.0), { 1.0, 1.0 } },
    { { LGL__RIGHT, LGL__DOWN, LGL__FORWARD }, vector3_forward(1.0), { 1.0, 0.0 } },
    { { LGL__LEFT,  LGL__DOWN, LGL__FORWARD }, vector3_forward(1.0), { 0.0, 0.0 } },

    { { LGL__LEFT,  LGL__DOWN, LGL__FORWARD }, vector3_forward(1.0), { 0.0, 0.0 } },
    { { LGL__LEFT,  LGL__UP,   LGL__FORWARD }, vector3_forward(1.0), { 0.0, 1.0 } },
    { { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, vector3_forward(1.0), { 1.0, 1.0 } },

    { { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, vector3_left(1.0),    { 0.0, 1.0 } },
    { { LGL__LEFT,  LGL__UP,   LGL__BACK    }, vector3_left(1.0),    { 1.0, 1.0 } },
    { { LGL__LEFT,  LGL__UP,   LGL__FORWARD }, vector3_left(1.0),    { 1.0, 0.0 } },

    { { LGL__LEFT,  LGL__UP,   LGL__FORWARD }, vector3_left(1.0),    { 1.0, 0.0 } },
    { { LGL__LEFT,  LGL__DOWN, LGL__FORWARD }, vector3_left(1.0),    { 0.0, 0.0 } },
    { { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, vector3_left(1.0),    { 0.0, 1.0 } },

    { { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, vector3_right(1.0),   { 1.0, 0.0 } },
    { { LGL__RIGHT, LGL__UP,   LGL__BACK    }, vector3_right(1.0),   { 1.0, 1.0 } },
    { { LGL__RIGHT, LGL__DOWN, LGL__BACK    }, vector3_right(1.0),   { 0.0, 1.0 } },

    { { LGL__RIGHT, LGL__DOWN, LGL__BACK    }, vector3_right(1.0),   { 0.0, 1.0 } },
    { { LGL__RIGHT, LGL__DOWN, LGL__FORWARD }, vector3_right(1.0),   { 0.0, 0.0 } },
    { { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, vector3_right(1.0),   { 1.0, 0.0 } },

    { { LGL__RIGHT, LGL__DOWN, LGL__FORWARD }, vector3_down(1.0),    { 1.0, 0.0 } },
    { { LGL__RIGHT, LGL__DOWN, LGL__BACK    }, vector3_down(1.0),    { 1.0, 1.0 } },
    { { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, vector3_down(1.0),    { 0.0, 1.0 } },

    { { LGL__LEFT,  LGL__DOWN, LGL__BACK    }, vector3_down(1.0),    { 0.0, 1.0 } },
    { { LGL__LEFT,  LGL__DOWN, LGL__FORWARD }, vector3_down(1.0),    { 0.0, 0.0 } },
    { { LGL__RIGHT, LGL__DOWN, LGL__FORWARD }, vector3_down(1.0),    { 1.0, 0.0 } },

    { { LGL__LEFT,  LGL__UP,   LGL__BACK    }, vector3_up(1.0),      { 0.0, 1.0 } },
    { { LGL__RIGHT, LGL__UP,   LGL__BACK    }, vector3_up(1.0),      { 1.0, 1.0 } },
    { { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, vector3_up(1.0),      { 1.0, 0.0 } },

    { { LGL__RIGHT, LGL__UP,   LGL__FORWARD }, vector3_up(1.0),      { 1.0, 0.0 } },
    { { LGL__LEFT,  LGL__UP,   LGL__FORWARD }, vector3_up(1.0),      { 0.0, 0.0 } },
    { { LGL__LEFT,  LGL__UP,   LGL__BACK    }, vector3_up(1.0),      { 0.0, 1.0 } },
  };

  cube.vertices       = cube_vertices;
  cube.vertex_count   = cube_vertices_count;

  cube.scale          = vector3_one(0.5);
  cube.position       = vector3_zero();
  cube.rotation       = quaternion_identity();

  cube.texture_offset = vector2_zero();
  cube.texture_scale  = vector2_one(1.0);

  cube.render_flags   = LGL_FLAG_ENABLED;

  lgl__buffer_vertex_array(
      &cube.VAO,
      &cube.VBO,
      cube.vertex_count,
      cube.vertices);

  return cube;
}

void lgl_framebuffer_alloc(
    lgl_framebuffer_t *frame,
    GLuint             shader) { 

  glGenFramebuffers(1, &frame->FBO);

  glBindFramebuffer(GL_FRAMEBUFFER, frame->FBO);

  glGenTextures(2, frame->color_buffers);

  for (unsigned int i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, frame->color_buffers[i]);

    {
      int width, height;
      glfwGetFramebufferSize(lgl__active_context->GLFWwindow, &width, &height);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height,
          0, GL_RGBA, GL_FLOAT, NULL);

      //debug_log("color_buffer %d size is %dx%d", i, width, height);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // attach texture to framebuffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
        GL_TEXTURE_2D, frame->color_buffers[i], 0);
  }

  GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, attachments);

  frame->quad = lgl_cube_alloc(); {
    frame->quad.shader        =  shader;
    frame->quad.diffuse_map   =  frame->color_buffers[0];
    //frame->quad.render_flags |= LGL_FLAG_USE_WIREFRAME;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void lgl__framebuffer_resize(const int width, const int height) { 

  glBindFramebuffer(GL_FRAMEBUFFER, lgl__active_framebuffer->FBO);

  glDeleteTextures(2, lgl__active_framebuffer->color_buffers);

  glGenTextures(2, lgl__active_framebuffer->color_buffers);
  for (unsigned int i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, lgl__active_framebuffer->color_buffers[i]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height,
        0, GL_RGBA, GL_FLOAT, NULL);

    //debug_log("color_buffer %d size is %dx%d", i, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // attach texture to framebuffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
        GL_TEXTURE_2D, lgl__active_framebuffer->color_buffers[i], 0);
  }

  GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, attachments);

  lgl__active_framebuffer->quad.diffuse_map   =  lgl__active_framebuffer->color_buffers[0];

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void lgl__framebuffer_size_callback(
    GLFWwindow* window,
    int         width,
    int         height) {
  (void)window;

  //debug_log("FBO %d cb0 %d cb1 %d",
  //    lgl__active_framebuffer->FBO,
  //    lgl__active_framebuffer->color_buffers[0],
  //    lgl__active_framebuffer->color_buffers[1]);

  glViewport(0, 0, width, height);

  lgl__framebuffer_resize(width, height);
}

void lgl__glfw_error_callback(int error, const char* description) {
  (void)error;
    fprintf(stderr, "Error: %s\n", description);
}

lgl_context_t *lgl_start(const int width, const int height) {
  debug_log("Rev up those fryers!");

  {
    lgl_context_t *context = malloc(sizeof(*context));
    lgl_active_context_set(context);
  }

  lgl__active_context->is_running     = 1;
  lgl__active_context->time_current   = 0;
  lgl__active_context->frame_current  = 0;
  lgl__active_context->time_delta     = 0;
  lgl__active_context->time_last      = 0;
  lgl__active_context->time_FPS       = 0;

  if (!glfwInit()) {
    debug_error("Failed to initialize GLFW!");
  }

  glfwSetErrorCallback(lgl__glfw_error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  lgl__active_context->GLFWwindow = glfwCreateWindow(width, height, "Game Window", NULL, NULL);
  if (!lgl__active_context->GLFWwindow) {
    debug_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(lgl__active_context->GLFWwindow);

  glfwSetFramebufferSizeCallback(
      lgl__active_context->GLFWwindow, lgl__framebuffer_size_callback);

  gladLoadGL(glfwGetProcAddress);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glStencilOp   (GL_KEEP, GL_KEEP, GL_REPLACE);
  glStencilFunc (GL_ALWAYS, 1, 0xFF);

  debug_log("Startup completed successfuly");

  debug_log("Success!");

  return lgl__active_context;
}

void lgl__time_update() {
  struct timespec spec;
  if (clock_gettime(CLOCK_MONOTONIC, &spec) != 0) {
    debug_error("failed to get time spec.");
    exit(0);
  }

  lgl__active_context->time_current  = spec.tv_sec + spec.tv_nsec * 1e-9;
  lgl__active_context->time_delta    = lgl__active_context->time_current - lgl__active_context->time_last;
  lgl__active_context->time_last     = lgl__active_context->time_current;
  lgl__active_context->time_FPS      = 1 / lgl__active_context->time_delta;
  lgl__active_context->frame_current++;

#if 0 // log time
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

void lgl_free(lgl_context_t *context) {
  debug_log("Shutting down...");

  context->is_running = 0;

  free     (context);
  debug_log("Shutdown complete");
}

void lgl_end_frame(lgl_context_t *context) {
  lgl__time_update();
  glfwPollEvents();
  glfwSwapBuffers(context->GLFWwindow);
}

