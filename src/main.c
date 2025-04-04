#include "lal.h"
#include "lgl.h"
#include "physics.h"

#define BLIB_IMPLEMENTATION
#include "blib/blib_math3d.h"

#include <stdio.h>

lgl_context *graphics_context;
GLuint shader_framebuffer = 0;
GLuint shader_solid = 0;
GLuint shader_phong = 0;
lgl_framebuffer framebuffer;
lgl_framebuffer framebuffer_MSAA;
l_object frame_obj;
lgl_light light;

void camera_update(lgl_context *context) {
  lgl_camera_update();

  { // movement
    vector3 movement = vector3_zero();
    {
      movement.y += glfwGetKey(context->GLFWwindow, GLFW_KEY_SPACE) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_LEFT_SHIFT);
      movement.x += glfwGetKey(context->GLFWwindow, GLFW_KEY_D) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_A);
      movement.z += glfwGetKey(context->GLFWwindow, GLFW_KEY_W) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_S);
      movement.x += glfwGetKey(context->GLFWwindow, GLFW_KEY_RIGHT) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_LEFT);
      movement.z += glfwGetKey(context->GLFWwindow, GLFW_KEY_UP) -
                    glfwGetKey(context->GLFWwindow, GLFW_KEY_DOWN);
    }
    float speed =
        glfwGetKey(context->GLFWwindow, GLFW_KEY_LEFT_CONTROL) ? 20 : 5;
    movement = vector3_normalize(movement);
    movement = vector3_scale(movement, context->time_delta * speed);
    movement = vector3_rotate(movement, context->camera.rotation);
    context->camera.position = vector3_add(context->camera.position, movement);
  }

#if 1
  { // mouse look
    static int firstFrame = 1;
    static float last_x = 0;
    static float last_y = 0;
    static float pitch = 0;
    static float yaw = 0;

    double mouse_x;
    double mouse_y;
    glfwGetCursorPos(context->GLFWwindow, &mouse_x, &mouse_y);

    if (firstFrame) {
      last_x = mouse_x;
      last_y = mouse_y;
      firstFrame = 0;
    }

    float diff_x = mouse_x - last_x;
    float diff_y = mouse_y - last_y;
    last_x = mouse_x;
    last_y = mouse_y;

    float sensitivity = 0.001;
    diff_x *= sensitivity;
    diff_y *= sensitivity;

    yaw += diff_x;
    pitch += diff_y;

    pitch = clamp(pitch, -PI * 0.5, PI * 0.5);

    quaternion rotation =
        quaternion_multiply(quaternion_from_euler(vector3_up(yaw)),
                            quaternion_from_euler(vector3_right(pitch)));
    context->camera.rotation = rotation;
  }
#endif
}

#include "demos/demo_galaxy.c"
#include "demos/demo_cube.c"
#include "demos/demo_icosphere.c"
#include "demos/demo_physics.c"

int main() {
  alutInit(0, 0);
  graphics_context = lgl_start(1000, 800);

  {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_texture_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_default_fragment.glsl", GL_FRAGMENT_SHADER);
    shader_framebuffer = lgl_shader_link(vertex_shader, fragment_shader);
  }

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

  // --------------------------------------------------------------------------
  // Create framebuffers

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

  graphics_context->camera = lgl_camera_alloc();
  graphics_context->camera.position.z = -25;

  light = (lgl_light){
      .type = 0,
      .position = {0.0, 0.0, -5},
      .direction = {0.0, 0.0, 1.0},
      .cut_off = cos(12.5),
      .outer_cut_off = cos(15.0),
      .constant = 1.0f,
      .linear = 0.09f,
      .quadratic = 0.032f,
      .diffuse = (vector3){1.0, 1.0, 1.0},
      .specular = vector3_one(0.6),
  };


#if 0
  demo_cube();
#endif

#if 1
  demo_physics();
#endif

#if 0
  demo_icosphere();
#endif

  lgl_camera_free(graphics_context->camera);
  l_object_free(frame_obj);
  lgl_framebuffer_free(framebuffer);
  lgl_framebuffer_free(framebuffer_MSAA);

  lgl_free(graphics_context);

  alutExit();

  return 0;
}
