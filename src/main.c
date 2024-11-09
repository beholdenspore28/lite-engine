#include "gl.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "blib/blib.h"
#include "blib/bmath.h"
#include "physics.h"

B_LIST_IMPLEMENTATION
DEFINE_LIST(vector3_t)
DEFINE_LIST(matrix4_t)
DEFINE_LIST(quaternion_t)

#define ASSERT_UNIMPLEMENTED 0

// debug toggles
#define ENGINE_SHOW_STATS_DRAW_CALLS 0
#define ENGINE_SHOW_STATS_TIME 0

static void error_callback(int error, const char *description) {
  (void)error;
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  (void)scancode;
  (void)mods;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static void framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                                   GLenum severity, GLsizei length,
                                   const char *message, const void *userParam) {
  (void)length;
  (void)userParam;

  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
    return;
  }

  printf("---------------\n");
  printf("Debug message (%d) %s\n", id, message);

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    printf("Source: API");
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    printf("Source: Window System");
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    printf("Source: Shader Compiler");
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    printf("Source: Third Party");
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    printf("Source: Application");
    break;
  case GL_DEBUG_SOURCE_OTHER:
    printf("Source: Other");
    break;
  }
  printf("\n");

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    printf("Type: Error");
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    printf("Type: Deprecated Behaviour");
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    printf("Type: Undefined Behaviour");
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    printf("Type: Portability");
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    printf("Type: Performance");
    break;
  case GL_DEBUG_TYPE_MARKER:
    printf("Type: Marker");
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    printf("Type: Push Group");
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    printf("Type: Pop Group");
    break;
  case GL_DEBUG_TYPE_OTHER:
    printf("Type: Other");
    break;
  }
  printf("\n");

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    printf("Severity: high");
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    printf("Severity: medium");
    break;
  case GL_DEBUG_SEVERITY_LOW:
    printf("Severity: low");
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    printf("Severity: notification");
    break;
  }
  printf("\n");
  printf("\n");
}

static GLFWwindow *engine_window;
static int engine_window_size_x = 640;
static int engine_window_size_y = 480;
static int engine_window_position_x = 0;
static int engine_window_position_y = 0;
static bool engine_window_fullscreen = false;
static char *engine_window_title = "Game Window";
static bool engine_window_always_on_top = false;
static float engine_time_current = 0.0f;
static float engine_time_last = 0.0f;
static float engine_time_delta = 0.0f;
static uint64_t engine_time_current_frame = 0;
static float engine_renderer_FPS = 0.0f;
static vector3_t engine_ambient_light = {0.01, 0.01, 0.01};
static engine_renderer_API_t engine_renderer_API = ENGINE_RENDERER_API_GL;
static camera_t engine_active_camera = {0};

void engine_renderer_set_API(engine_renderer_API_t renderingAPI) {
  engine_renderer_API = renderingAPI;
}

// set window resolution
void engine_window_set_resolution(int x, int y) {
  glfwSetWindowSize(engine_window, x, y);
}

// position window in the center of the screen
void engine_window_set_position(int x, int y) {
  glfwSetWindowPos(engine_window, x, y);
}

void engine_start_renderer_api_gl(void) {
  if (!glfwInit()) {
    printf("[ERROR_GLFW] Failed to initialize GLFW");
  }

  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  if (engine_window_always_on_top)
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

  assert(engine_window_title != NULL);

  if (engine_window_fullscreen)
    engine_window =
        glfwCreateWindow(engine_window_size_x, engine_window_size_y,
                         engine_window_title, glfwGetPrimaryMonitor(), NULL);
  else
    engine_window = glfwCreateWindow(engine_window_size_x, engine_window_size_y,
                                     engine_window_title, NULL, NULL);

  assert(engine_window != NULL);

  glfwSetWindowPos(engine_window, engine_window_position_x,
                   engine_window_position_y);
  glfwShowWindow(engine_window);
  glfwMakeContextCurrent(engine_window);
  glfwSetKeyCallback(engine_window, key_callback);
  glfwSetFramebufferSizeCallback(engine_window, framebuffer_size_callback);
  glfwSetInputMode(engine_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGL()) {
    printf("[ERROR_GL] Failed to initialize GLAD\n");
  }

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
    printf("[ERROR_GL] Failed to set debug context flag\n");
  } else {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                          GL_TRUE);
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  int width, height;
  glfwGetFramebufferSize(engine_window, &width, &height);
  glViewport(0, 0, width, height);

  glfwSwapInterval(0);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(engine_window);

  engine_active_camera = (camera_t){
      .transform.position = (vector3_t){0.0, 0.0, -5.0},
      .transform.rotation = quaternion_identity(),
      .transform.scale = vector3_one(1.0),
      .projection = matrix4_identity(),
      .lookSensitivity = 0.002f,
  };
}

void engine_start(void) {
  switch (engine_renderer_API) {
  case ENGINE_RENDERER_API_GL:
    engine_start_renderer_api_gl();
    break;
  default:
    assert(0);
    break;
  }
}

void engine_set_clear_color(float r, float g, float b, float a) {
  switch (engine_renderer_API) {
  case ENGINE_RENDERER_API_GL:
    glClearColor((GLfloat)r, (GLfloat)g, (GLfloat)b, (GLfloat)a);
    break;
  case ENGINE_RENDERER_API_NONE:
    break;
  }
}

static inline void transform_calculate_matrix(transform_t *t) {
  matrix4_t translation = matrix4_translate(t->position);
  matrix4_t rotation = quaternion_to_matrix4(t->rotation);
  matrix4_t scale = matrix4_scale(t->scale);
  t->matrix = matrix4_multiply(rotation, translation);
  t->matrix = matrix4_multiply(scale, t->matrix);
}

static inline void transform_calculate_view_matrix(transform_t *t) {
  matrix4_t translation = matrix4_translate(vector3_negate(t->position));
  matrix4_t rotation = quaternion_to_matrix4(quaternion_conjugate(t->rotation));
  matrix4_t scale = matrix4_scale(t->scale);
  t->matrix = matrix4_multiply(translation, rotation);
  t->matrix = matrix4_multiply(scale, t->matrix);
}

static inline vector3_t transform_basis_forward(transform_t t,
                                                float magnitude) {
  return vector3_rotate(vector3_forward(magnitude), t.rotation);
}

static inline vector3_t transform_basis_up(transform_t t, float magnitude) {
  return vector3_rotate(vector3_up(magnitude), t.rotation);
}

static inline vector3_t transform_basis_right(transform_t t, float magnitude) {
  return vector3_rotate(vector3_right(magnitude), t.rotation);
}

static inline vector3_t transform_basis_back(transform_t t, float magnitude) {
  return vector3_rotate(vector3_back(magnitude), t.rotation);
}

static inline vector3_t transform_basis_down(transform_t t, float magnitude) {
  return vector3_rotate(vector3_down(magnitude), t.rotation);
}

static inline vector3_t transform_basis_left(transform_t t, float magnitude) {
  return vector3_rotate(vector3_left(magnitude), t.rotation);
}

DECLARE_LIST(transform_t)
DEFINE_LIST(transform_t)

DEFINE_LIST(primitive_shape_t)

uint32_t drawCallsSaved = 0;

typedef struct {
  vector3_t position;
  float constant;
  float linear;
  float quadratic;
  vector3_t diffuse;
  vector3_t specular;
} pointLight_t;

pointLight_t light;

void quad_draw(quad_t *quad) {
  if (quad->transform.scale.x < FLOAT_EPSILON &&
      quad->transform.scale.y < FLOAT_EPSILON &&
      quad->transform.scale.z < FLOAT_EPSILON) {
    drawCallsSaved++;
    return;
  }
  glUseProgram(quad->material.shader);

  // model matrix
  transform_calculate_matrix(&quad->transform);

  shader_setUniformM4(quad->material.shader, "u_modelMatrix",
                      &quad->transform.matrix);

  // view matrix
  shader_setUniformM4(quad->material.shader, "u_viewMatrix",
                      &engine_active_camera.transform.matrix);

  // projection matrix
  shader_setUniformM4(quad->material.shader, "u_projectionMatrix",
                      &engine_active_camera.projection);

  // material
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, quad->material.diffuseMap);
  vector3_t color = {1.0f, 0.0f, 0.0f};
  shader_setUniformV3(quad->material.shader, "u_color", color);
  shader_setUniformInt(quad->material.shader, "u_texture", 0);

  glBindVertexArray(quad->mesh.VAO);
  glDrawElements(GL_TRIANGLES, MESH_QUAD_NUM_INDICES, GL_UNSIGNED_INT, 0);
}

void sphere_draw(primitive_shape_t *sphere) {
  if (sphere->transform.scale.x < FLOAT_EPSILON &&
      sphere->transform.scale.y < FLOAT_EPSILON &&
      sphere->transform.scale.z < FLOAT_EPSILON) {
    drawCallsSaved++;
    return;
  }
  glUseProgram(sphere->material.shader);

  // light uniforms
  shader_setUniformV3(sphere->material.shader, "u_pointLight.position",
                      light.position);
  shader_setUniformFloat(sphere->material.shader, "u_pointLight.constant",
                         light.constant);
  shader_setUniformFloat(sphere->material.shader, "u_pointLight.linear",
                         light.linear);
  shader_setUniformFloat(sphere->material.shader, "u_pointLight.quadratic",
                         light.quadratic);
  shader_setUniformV3(sphere->material.shader, "u_pointLight.diffuse",
                      light.diffuse);
  shader_setUniformV3(sphere->material.shader, "u_pointLight.specular",
                      light.specular);

  // model matrix
  transform_calculate_matrix(&sphere->transform);

  shader_setUniformM4(sphere->material.shader, "u_modelMatrix",
                      &sphere->transform.matrix);

  // view matrix
  shader_setUniformM4(sphere->material.shader, "u_viewMatrix",
                      &engine_active_camera.transform.matrix);

  // projection matrix
  shader_setUniformM4(sphere->material.shader, "u_projectionMatrix",
                      &engine_active_camera.projection);

  // camera position
  shader_setUniformV3(sphere->material.shader, "u_cameraPos",
                      engine_active_camera.transform.position);

  // material
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sphere->material.diffuseMap);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, sphere->material.specularMap);

  shader_setUniformInt(sphere->material.shader, "u_material.diffuse", 0);
  shader_setUniformInt(sphere->material.shader, "u_material.specular", 1);
  shader_setUniformFloat(sphere->material.shader, "u_material.shininess",
                         32.0f);
  shader_setUniformV3(sphere->material.shader, "u_ambientLight",
                      engine_ambient_light);

  glBindVertexArray(sphere->mesh.VAO);
  glDrawElements(GL_TRIANGLES, sphere->mesh.indexCount, GL_UNSIGNED_INT, 0);
}

void cube_draw(primitive_shape_t *cube) {
  if (cube->transform.scale.x < FLOAT_EPSILON &&
      cube->transform.scale.y < FLOAT_EPSILON &&
      cube->transform.scale.z < FLOAT_EPSILON) {
    drawCallsSaved++;
    return;
  }
  glUseProgram(cube->material.shader);

  // light uniforms
  shader_setUniformV3(cube->material.shader, "u_pointLight.position",
                      light.position);
  shader_setUniformFloat(cube->material.shader, "u_pointLight.constant",
                         light.constant);
  shader_setUniformFloat(cube->material.shader, "u_pointLight.linear",
                         light.linear);
  shader_setUniformFloat(cube->material.shader, "u_pointLight.quadratic",
                         light.quadratic);
  shader_setUniformV3(cube->material.shader, "u_pointLight.diffuse",
                      light.diffuse);
  shader_setUniformV3(cube->material.shader, "u_pointLight.specular",
                      light.specular);

  // model matrix
  transform_calculate_matrix(&cube->transform);

  shader_setUniformM4(cube->material.shader, "u_modelMatrix",
                      &cube->transform.matrix);

  // view matrix
  shader_setUniformM4(cube->material.shader, "u_viewMatrix",
                      &engine_active_camera.transform.matrix);

  // projection matrix
  shader_setUniformM4(cube->material.shader, "u_projectionMatrix",
                      &engine_active_camera.projection);

  // camera position
  shader_setUniformV3(cube->material.shader, "u_cameraPos",
                      engine_active_camera.transform.position);

  // material
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, cube->material.diffuseMap);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, cube->material.specularMap);

  shader_setUniformInt(cube->material.shader, "u_material.diffuse", 0);
  shader_setUniformInt(cube->material.shader, "u_material.specular", 1);
  shader_setUniformFloat(cube->material.shader, "u_material.shininess", 32.0f);
  shader_setUniformV3(cube->material.shader, "u_ambientLight",
                      engine_ambient_light);

  glBindVertexArray(cube->mesh.VAO);
  glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
}

#if 0
mesh_t mesh_alloc_planet(const int subDivisions, const float radius) {
  list_vertex_t vertices = list_vertex_t_alloc();
  list_uint32_t indices = list_uint32_t_alloc();
  int index = 0;
  enum { FACE_FRONT, FACE_BACK, FACE_RIGHT, FACE_LEFT, FACE_UP, FACE_DOWN };
  for (int faceDirection = 0; faceDirection < 6; faceDirection++) {
    int offset = subDivisions * subDivisions * faceDirection;
    for (int k = 0; k < subDivisions; k++) {
      for (int j = 0; j < subDivisions; j++) {
        vector3_t point = {
            map(k, 0, subDivisions - 1, -1, 1),
            map(j, 0, subDivisions - 1, -1, 1),
            1,
        };
        if (k < subDivisions - 1 && j < subDivisions - 1) {
          int first = offset + (k * (subDivisions)) + j;
          int second = first + subDivisions;
          for (int i = 0; i < 6; i++) {
            list_uint32_t_add(&indices, 0);
          }
          indices.data[index++] = first + 1;
          indices.data[index++] = second;
          indices.data[index++] = first;
          indices.data[index++] = first + 1;
          indices.data[index++] = second + 1;
          indices.data[index++] = second;
        }
        vector3_t original = point;
        switch (faceDirection) {
        case FACE_FRONT: {
          point.x = original.x;
          point.y = original.y;
          point.z = original.z;
        } break;
        case FACE_BACK: {
          point.x = original.x;
          point.y = -original.y;
          point.z = -original.z;
        } break;
        case FACE_RIGHT: {
          point.x = original.z;
          point.y = -original.y;
          point.z = original.x;
        } break;
        case FACE_LEFT: {
          point.x = -original.z;
          point.y = original.y;
          point.z = original.x;
        } break;
        case FACE_UP: {
          point.x = original.x;
          point.y = original.z;
          point.z = -original.y;
        } break;
        case FACE_DOWN: {
          point.x = -original.x;
          point.y = -original.z;
          point.z = -original.y;
        } break;
        default:
          break;
        }

        vector3_t pointOnSphere = vector3_normalize(point); // <<== THIS IS WHY ITS NOT ROUND

        float freq = 10,
              amp = 1.0,
              offset = 10;

        float wave = noise3_perlin(
            pointOnSphere.x*freq + offset, 
            pointOnSphere.y*freq + offset, 
            pointOnSphere.z*freq + offset) * amp;

        float u = map(k, 0, subDivisions - 1, 0, 1),
              v = map(j, 0, subDivisions - 1, 0, 1);
#if 1
        if (wave < 0.02) {
          wave = wave = 0.02;
        }
#endif
        vertex_t vertex = {
            .position = vector3_scale(pointOnSphere, wave + radius * 0.5),
            .normal = pointOnSphere,
            .texCoord = {u, v},
        };
        list_vertex_t_add(&vertices, vertex);
      }
    }
  }
  mesh_t m = mesh_alloc(&vertices.data[0], &indices.data[0], vertices.length,
                        indices.length);
  m.vertices = vertices;
  m.indices = indices;
  return m;
}
#else
mesh_t mesh_alloc_planet(const int subDivisions, const float radius) {
  list_vertex_t vertices = list_vertex_t_alloc();
  list_uint32_t indices = list_uint32_t_alloc();
  int index = 0;
  enum { FACE_FRONT, FACE_BACK, FACE_RIGHT, FACE_LEFT, FACE_UP, FACE_DOWN };
  for (int faceDirection = 0; faceDirection < 6; faceDirection++) {
    int offset = subDivisions * subDivisions * faceDirection;
    for (int k = 0; k < subDivisions; k++) {
      for (int j = 0; j < subDivisions; j++) {
        vector3_t point = {
            map(k, 0, subDivisions - 1, -1, 1),
            map(j, 0, subDivisions - 1, -1, 1),
            1,
        };
        if (k < subDivisions - 1 && j < subDivisions - 1) {
          int first = offset + (k * (subDivisions)) + j;
          int second = first + subDivisions;
          for (int i = 0; i < 6; i++) {
            list_uint32_t_add(&indices, 0);
          }
          indices.data[index++] = first + 1;
          indices.data[index++] = second;
          indices.data[index++] = first;
          indices.data[index++] = first + 1;
          indices.data[index++] = second + 1;
          indices.data[index++] = second;
        }
        vector3_t original = point;
        switch (faceDirection) {
        case FACE_FRONT: {
          point.x = original.x;
          point.y = original.y;
          point.z = original.z;
        } break;
        case FACE_BACK: {
          point.x = original.x;
          point.y = -original.y;
          point.z = -original.z;
        } break;
        case FACE_RIGHT: {
          point.x = original.z;
          point.y = -original.y;
          point.z = original.x;
        } break;
        case FACE_LEFT: {
          point.x = -original.z;
          point.y = original.y;
          point.z = original.x;
        } break;
        case FACE_UP: {
          point.x = original.x;
          point.y = original.z;
          point.z = -original.y;
        } break;
        case FACE_DOWN: {
          point.x = -original.x;
          point.y = -original.z;
          point.z = -original.y;
        } break;
        default:
          break;
        }

        vector3_t pointOnSphere = vector3_normalize(point); // <<== THIS IS WHY ITS NOT ROUND

        float freq = 10,
              amp = 1.0,
              offset = 10;

        float wave = noise3_perlin(
            pointOnSphere.x*freq + offset, 
            pointOnSphere.y*freq + offset, 
            pointOnSphere.z*freq + offset) * amp;

        float u = map(k, 0, subDivisions - 1, 0, 1),
              v = map(j, 0, subDivisions - 1, 0, 1);
#if 1
        if (wave < 0.02) {
          wave = wave = 0.02;
        }
#endif
        vertex_t vertex = {
            .position = vector3_scale(pointOnSphere, wave + radius * 0.5),
            .normal = pointOnSphere,
            .texCoord = {u, v},
        };
        list_vertex_t_add(&vertices, vertex);
      }
    }
  }
  mesh_t m = mesh_alloc(&vertices.data[0], &indices.data[0], vertices.length,
                        indices.length);
  m.vertices = vertices;
  m.indices = indices;
  return m;
}
#endif
mesh_t mesh_alloc_cube_sphere(const int subDivisions, const float radius) {
  list_vertex_t vertices = list_vertex_t_alloc();
  list_uint32_t indices = list_uint32_t_alloc();
  int index = 0;
  for (int faceDirection = 0; faceDirection < 6; faceDirection++) {
    int offset = subDivisions * subDivisions * faceDirection;
    for (int k = 0; k < subDivisions; k++) {
      for (int j = 0; j < subDivisions; j++) {
        vector3_t point = {
            map(k, 0, subDivisions - 1, -1, 1),
            map(j, 0, subDivisions - 1, -1, 1),
            1,
        };
        if (k < subDivisions - 1 && j < subDivisions - 1) {
          int first = offset + (k * (subDivisions)) + j;
          int second = first + subDivisions;
          for (int i = 0; i < 6; i++) {
            list_uint32_t_add(&indices, 0);
          }
          indices.data[index++] = first + 1;
          indices.data[index++] = second;
          indices.data[index++] = first;
          indices.data[index++] = first + 1;
          indices.data[index++] = second + 1;
          indices.data[index++] = second;
        }

        enum {
          FACE_FRONT,
          FACE_BACK,
          FACE_RIGHT,
          FACE_LEFT,
          FACE_UP,
          FACE_DOWN,
        };

        vector3_t original = point;
        switch (faceDirection) {
        case FACE_FRONT: {
          point.x = original.x;
          point.y = original.y;
          point.z = original.z;
        } break;
        case FACE_BACK: {
          point.x = original.x;
          point.y = -original.y;
          point.z = -original.z;
        } break;
        case FACE_RIGHT: {
          point.x = original.z;
          point.y = -original.y;
          point.z = original.x;
        } break;
        case FACE_LEFT: {
          point.x = -original.z;
          point.y = original.y;
          point.z = original.x;
        } break;
        case FACE_UP: {
          point.x = original.x;
          point.y = original.z;
          point.z = -original.y;
        } break;
        case FACE_DOWN: {
          point.x = -original.x;
          point.y = -original.z;
          point.z = -original.y;
        } break;
        default:
          break;
        }
        float u = map(k, 0, subDivisions - 1, 0, 1),
              v = map(j, 0, subDivisions - 1, 0, 1);
        vector3_t normal = vector3_normalize(point);
        vertex_t vertex = {
            .position = vector3_scale(normal, radius * 0.5),
            .normal = normal,
            .texCoord = {u, v},
        };
        list_vertex_t_add(&vertices, vertex);
      }
    }
  }
  mesh_t m = mesh_alloc(&vertices.data[0], &indices.data[0], vertices.length,
                        indices.length);
  m.vertices = vertices;
  m.indices = indices;
  return m;
}

mesh_t mesh_alloc_sphere(const int latCount, const int lonCount,
                         const float radius) {
  const float halfRadius = radius * 0.5;
  list_vertex_t vertices = list_vertex_t_alloc();
  for (int lat = 0; lat <= latCount; lat++) {
    float theta = PI * lat / latCount;
    for (int lon = 0; lon <= lonCount; lon++) {
      float phi = 2 * PI * lon / lonCount;

      float x = halfRadius * sin(theta) * cos(phi), y = halfRadius * cos(theta),
            z = halfRadius * sin(theta) * sin(phi),
            u = map(lon, 0, lonCount, 0, 1), v = -map(lat, 0, latCount, 0, 1);

      vector3_t point = {x, y, z};
      vector2_t texCoord = {u, v};
      vector3_t normal = vector3_normalize(point);

      vertex_t newVert = {
          .position = point,
          .texCoord = texCoord,
          .normal = normal,
      };
      list_vertex_t_add(&vertices, newVert);
    }
  }

  list_uint32_t indices = list_uint32_t_alloc();
  int index = 0;
  for (int lat = 0; lat < latCount; lat++) {
    for (int lon = 0; lon < lonCount; lon++) {
      int first = (lat * (lonCount + 1)) + lon;
      int second = first + lonCount + 1;
      for (int i = 0; i < 6; i++) {
        list_uint32_t_add(&indices, 0);
      }
      indices.data[index++] = first;
      indices.data[index++] = second;
      indices.data[index++] = first + 1;
      indices.data[index++] = second;
      indices.data[index++] = second + 1;
      indices.data[index++] = first + 1;
    }
  }

  mesh_t m = mesh_alloc(&vertices.data[0], &indices.data[0], vertices.length,
                        indices.length);
  m.vertices = vertices;
  m.indices = indices;
  return m;
}

void mesh_free(mesh_t *m) {
  list_vertex_t_free(&m->vertices);
  list_uint32_t_free(&m->indices);
}

int main(void) {
  printf("Rev up those fryers!\n");

  engine_window_title = "Game Window";
  engine_renderer_set_API(ENGINE_RENDERER_API_GL);
  engine_window_size_x = 1920;
  engine_window_size_y = 1080;
  engine_window_position_x = 0;
  engine_window_position_y = 0;
  // engine_window_fullscreen = true;
  // engine_window_always_on_top = true;
  engine_start();

  engine_set_clear_color(0.0, 0.0, 0.0, 1.0);

  // shader creation.
  GLuint diffuseShader = shader_create("res/shaders/diffuse.vs.glsl",
                                       "res/shaders/diffuse.fs.glsl");
  GLuint unlitShader =
      shader_create("res/shaders/unlit.vs.glsl", "res/shaders/unlit.fs.glsl");

  // skybox creation
  GLuint skyboxDiffuseMap = texture_create("res/textures/space.png");

  vertex_t *vertices = mesh_cube_vertices;
  GLuint *indices = mesh_cube_indices_reversed;

  mesh_t skyboxMesh = mesh_alloc(vertices, indices, MESH_CUBE_NUM_VERTICES,
                                 MESH_CUBE_NUM_INDICES);
  primitive_shape_t skybox = {
      .transform.position = (vector3_t){0.0f, 0.0f, 0.0f},
      .transform.rotation = quaternion_identity(),
      .transform.scale = vector3_one(1000.0),
      .mesh = skyboxMesh,
      .material = {
        .shader = unlitShader,
        .diffuseMap = skyboxDiffuseMap,
        .specularMap = 0,
      },
  };

  GLuint cubeDiffuseMap = texture_create("res/textures/lunarrock_d.png");

  primitive_shape_t planet = (primitive_shape_t){
      .transform.position = (vector3_t){0, -20, 100},
      .transform.rotation = quaternion_identity(),
      .transform.scale = vector3_one(100.0),
      .mesh = mesh_alloc_planet(20, 1),
      .material = {
              .shader = diffuseShader,
              .diffuseMap = cubeDiffuseMap,
          },
  };

  // create point light
  light = (pointLight_t){
      .position = (vector3_t){0.0f, 10.0f, -10.0f},
      .diffuse = vector3_one(0.8f),
      .specular = vector3_one(1.0f),
      .constant = 1.0f,
      .linear = 0.09f,     // changed from 0.09
      .quadratic = 0.032f, // changed from 0.032
  };

  vector3_t mouseLookVector = vector3_zero();

  while (!glfwWindowShouldClose(engine_window)) {
    { // TIME
      engine_time_current = glfwGetTime();
      engine_time_delta = engine_time_current - engine_time_last;
      engine_time_last = engine_time_current;

      engine_renderer_FPS = 1 / engine_time_delta;
      engine_time_current_frame++;
#if ENGINE_SHOW_STATS_TIME
      printf("============FRAME=START==============\n");
      printf("DELTA_TIME: %f | FPS: %f | CURRENT_FRAME %d\n", engine_time_delta,
             engine_renderer_FPS, engine_time_current_frame);
#endif // ENGINE_SHOW_STATS_TIME
    }  // END TIME

    {   // INPUT
      { // mouse look
        static bool firstMouse = true;
        double mouseX, mouseY;
        glfwGetCursorPos(engine_window, &mouseX, &mouseY);

        if (firstMouse) {
          engine_active_camera.lastX = mouseX;
          engine_active_camera.lastY = mouseY;
          firstMouse = false;
        }

        float xoffset = mouseX - engine_active_camera.lastX;
        float yoffset = mouseY - engine_active_camera.lastY;
        engine_active_camera.lastX = mouseX;
        engine_active_camera.lastY = mouseY;

        mouseLookVector.x += yoffset * engine_active_camera.lookSensitivity;
        mouseLookVector.y += xoffset * engine_active_camera.lookSensitivity;

        mouseLookVector.y = loop(mouseLookVector.y, 2 * PI);
        mouseLookVector.x = clamp(mouseLookVector.x, -PI * 0.5, PI * 0.5);

        vector3_scale(mouseLookVector, engine_time_delta);

        engine_active_camera.transform.rotation =
            quaternion_from_euler(mouseLookVector);
      }

      { // movement
        float cameraSpeed = 16 * engine_time_delta;
        float cameraSpeedCurrent;
        if (glfwGetKey(engine_window, GLFW_KEY_LEFT_CONTROL)) {
          cameraSpeedCurrent = 4 * cameraSpeed;
        } else {
          cameraSpeedCurrent = cameraSpeed;
        }
        vector3_t movement = vector3_zero();

        movement.x = glfwGetKey(engine_window, GLFW_KEY_D) -
                     glfwGetKey(engine_window, GLFW_KEY_A);
        movement.y = glfwGetKey(engine_window, GLFW_KEY_SPACE) -
                     glfwGetKey(engine_window, GLFW_KEY_LEFT_SHIFT);
        movement.z = glfwGetKey(engine_window, GLFW_KEY_W) -
                     glfwGetKey(engine_window, GLFW_KEY_S);

        movement = vector3_normalize(movement);
        movement = vector3_scale(movement, cameraSpeedCurrent);
        movement =
            vector3_rotate(movement, engine_active_camera.transform.rotation);

        engine_active_camera.transform.position =
            vector3_add(engine_active_camera.transform.position, movement);

        if (glfwGetKey(engine_window, GLFW_KEY_BACKSPACE)) {
          engine_active_camera.transform.position = vector3_zero();
        }
        if (glfwGetKey(engine_window, GLFW_KEY_X)) {
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
      }
    } // END INPUT

    // projection
    glfwGetWindowSize(engine_window, &engine_window_size_x,
                      &engine_window_size_y);
    float aspect = (float)engine_window_size_x / (float)engine_window_size_y;
    engine_active_camera.projection =
        matrix4_perspective(deg2rad(60), aspect, 0.0001f, 1000.0f);

    { // draw
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      engine_active_camera.transform.matrix = matrix4_identity();
      glDisable(GL_DEPTH_TEST);
      skybox.transform.rotation =
          quaternion_conjugate(engine_active_camera.transform.rotation);
      cube_draw(&skybox);
      glEnable(GL_DEPTH_TEST);

      transform_calculate_view_matrix(&engine_active_camera.transform);

      quaternion_t rotation = quaternion_from_euler(vector3_up(engine_time_delta*0.03));
      planet.transform.rotation = quaternion_multiply(planet.transform.rotation, rotation);
      sphere_draw(&planet);

      glfwSwapBuffers(engine_window);
      glfwPollEvents();

#if ENGINE_SHOW_STATS_DRAW_CALLS
      if (drawCallsSaved > 0) {
        printf("saved %d draw calls this frame\n", drawCallsSaved);
        drawCallsSaved = 0;
      }
#endif // ENGINE_SHOW_STATS_DRAW_CALLS
    }
  }
  mesh_free(&planet.mesh);

  glfwTerminate();
  return 0;
}
