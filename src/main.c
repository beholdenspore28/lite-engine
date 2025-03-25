#include "lgl.h"
#define BLIB_IMPLEMENTATION
#include "blib/blib_math3d.h"

void camera_update(lgl_context_t *context) {
  { // movement
    vector3_t movement = vector3_zero(); {
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
    float speed = glfwGetKey(context->GLFWwindow, GLFW_KEY_LEFT_CONTROL) ? 10 : 1;
    movement = vector3_normalize(movement);
    movement = vector3_scale(movement, context->time_delta * speed);
    movement = vector3_rotate(movement, context->camera.rotation);
    context->camera.position = vector3_add(context->camera.position, movement);
  }

  // mouse look
  if (glfwGetMouseButton(context->GLFWwindow, GLFW_MOUSE_BUTTON_1)) {
    static int firstFrame = 1;
    static float last_x   = 0;
    static vector3_t eulerAngles = {0};

    double mouse_x, mouse_y;

    glfwGetCursorPos(context->GLFWwindow, &mouse_x, &mouse_y);

    if (firstFrame) {
      last_x = mouse_x;
      firstFrame = 0;
    }

    float diff_x = mouse_x - last_x;
    last_x = mouse_x;

    eulerAngles.y = diff_x * 0.01;
    quaternion_t rotation = quaternion_from_euler(eulerAngles);
    context->camera.rotation = quaternion_multiply(context->camera.rotation, rotation);
  }
}

void galaxy_distribution(
    lgl_object_t       stars,
    float              radius,
    unsigned int       seed,
    float              swirl_strength,
    float              arm_thickness,
    float              arm_length) {

  for(unsigned int i = 0; i < stars.length; i++) {

    // sphere
    stars.position[i] = vector3_point_in_unit_sphere(seed + i);

    // gravity
    vector3_t gravity = vector3_normalize(stars.position[i]);
    stars.position[i] = vector3_subtract(stars.position[i], gravity);

    // stretch
    stars.position[i].x *= arm_thickness;
    stars.position[i].z *= arm_length;
    
    // swirl
    float swirl_amount = vector3_square_magnitude(stars.position[i]) * swirl_strength;
    stars.position[i] = vector3_rotate(stars.position[i], quaternion_from_euler(vector3_up(swirl_amount)));

    // scale
    stars.position[i] = vector3_scale(stars.position[i], radius);

  }
}

void matrix_buffer(lgl_object_t *object) {
  GLfloat model_matrices[object->length][16];
  for(unsigned int i = 0; i < object->length; i++) {

    lgl_mat4_identity(model_matrices[i]);

    {
      GLfloat scale[16]; lgl_mat4_identity(scale);
      scale[0 ] = object->scale[i].x;
      scale[5 ] = object->scale[i].y;
      scale[10] = object->scale[i].z;

      GLfloat translation[16]; lgl_mat4_identity(translation);
      translation[12] = object->position[i].x;
      translation[13] = object->position[i].y;
      translation[14] = object->position[i].z;

      GLfloat rotation[16] = {0};
      quaternion_to_mat4(object->rotation[i], rotation);

      lgl_mat4_multiply(model_matrices[i], scale, rotation);
      lgl_mat4_multiply(model_matrices[i], model_matrices[i], translation);
    }
  }

  // --------------------------------------------------------------------------
  // configure instanced array

  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  glBufferData(
      GL_ARRAY_BUFFER, object->length * sizeof(GLfloat) * 16,
      &model_matrices[0], GL_STATIC_DRAW);

  glBindVertexArray(object->VAO);

  // set attribute pointers for matrix (4 times vec4)
  glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE,
      sizeof(GLfloat)*16, (void*)0);

  glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE,
      sizeof(GLfloat)*16, (void*)(1 * sizeof(vector4_t)));

  glVertexAttribPointer( 5, 4, GL_FLOAT, GL_FALSE,
      sizeof(GLfloat)*16, (void*)(2 * sizeof(vector4_t)));

  glVertexAttribPointer( 6, 4, GL_FLOAT, GL_FALSE,
      sizeof(GLfloat)*16, (void*)(3 * sizeof(vector4_t)));

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

int main() {
  lgl_context_t *context = lgl_start(640, 480);

  glClearColor(0,0,0,1);

  // --------------------------------------------------------------------------
  // Create shaders
  
  GLuint shader_solid = 0; {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/solid_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/solid_fragment.glsl",
        GL_FRAGMENT_SHADER);

    shader_solid = lgl_shader_link(vertex_shader, fragment_shader);
  }

  GLuint shader_framebuffer = 0; {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_texture_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/frame_buffer_default_fragment.glsl",
        GL_FRAGMENT_SHADER);

    shader_framebuffer = lgl_shader_link(vertex_shader, fragment_shader);
  }

  // --------------------------------------------------------------------------
  // Create lights

  enum {
    LIGHTS_POINT_0,
    LIGHTS_POINT_1,
    LIGHTS_COUNT, // this should ALWAYS be at the end of the enum
  };
  lgl_light_t lights [LIGHTS_COUNT] = {0};

  lights[LIGHTS_POINT_0] = (lgl_light_t) {
    .type           = 0,
    .position       = {0.0, 1.0, -5.0},
    .direction      = {0.0, 0.0, 1.0},
    .cut_off        = cos(12.5),
    .outer_cut_off  = cos(15.0),
    .constant       = 1.0f,
    .linear         = 0.09f,
    .quadratic      = 0.032f,
    .diffuse        = (vector3_t){1.0, 0.0, 0.0},
    .specular       = vector3_one(0.6),
  };

  lights[LIGHTS_POINT_1] = (lgl_light_t) {
    .type           = 0,
    .position       = {0.0, 1.0, -5.0},
    .direction      = {0.0, 0.0, 1.0},
    .cut_off        = cos(12.5),
    .outer_cut_off  = cos(15.0),
    .constant       = 1.0f,
    .linear         = 0.09f,
    .quadratic      = 0.032f,
    .diffuse        = (vector3_t){0.0, 0.0, 1.0},
    .specular       = vector3_one(0.6),
  };

  // --------------------------------------------------------------------------
  // Create stars

  
  lgl_object_t stars = lgl_object_alloc(10000, LGL_OBJECT_ARCHETYPE_CUBE); 
  stars.shader = shader_solid;
  stars.color = (vector4_t) { 1.0, 1.0, 1.0, 1.0 };
  stars.render_flags |= LGL_FLAG_USE_WIREFRAME;
  
  lgl_object_t stars_blue = lgl_object_alloc(10000, LGL_OBJECT_ARCHETYPE_CUBE); 
  stars_blue.shader = shader_solid;
  stars_blue.color = (vector4_t) { 1.0, 0.5, 0.5, 1.0 };
  stars_blue.render_flags |= LGL_FLAG_USE_WIREFRAME;
  
  for(unsigned int i = 0; i < stars.length; i++) {
    stars_blue.scale[i] = vector3_one(0.04);
    stars.scale[i]      = vector3_one(0.02);
  }
  
  {
    float radius         = 10;
    float swirl_strength = 0.3;
    float arm_thickness  = 3;
    float arm_length     = 5;
    
    galaxy_distribution(stars,      radius, 902347, swirl_strength, arm_thickness, arm_length);
    galaxy_distribution(stars_blue, radius, 0,      swirl_strength, arm_thickness, arm_length);
  }
  
  for(unsigned int i = 0; i < stars.length; i++) {
    stars.position[i] = vector3_rotate(stars.position[i], quaternion_from_euler(vector3_right(-PI/8)));
    stars_blue.position[i] = vector3_rotate(stars_blue.position[i], quaternion_from_euler(vector3_right(-PI/8)));
  }
  
  matrix_buffer(&stars);
  matrix_buffer(&stars_blue);
  
  stars.position[0] = vector3_zero();
  stars.rotation[0] = quaternion_identity();
  stars_blue.position[0] = vector3_zero();
  stars_blue.rotation[0] = quaternion_identity();
  
  // --------------------------------------------------------------------------
  // Create framebuffer
  
  lgl_framebuffer_t frame = lgl_framebuffer_alloc(shader_framebuffer);
  lgl_active_framebuffer_set(&frame);

  // --------------------------------------------------------------------------
  // Create camera

  context->camera.rotation   = quaternion_identity();
  context->camera.position   = vector3_zero();
  context->camera.position.z = -50;

  GLfloat view[16];          lgl_mat4_identity(view);
  GLfloat projection[16];    lgl_mat4_identity(projection);

  context->camera.view       = view;
  context->camera.projection = projection;

  // --------------------------------------------------------------------------
  // game loop
  
  float timer = 0;

  while(!glfwWindowShouldClose(context->GLFWwindow)) {

    lgl_camera_update();

    timer += context->time_delta;

    if (timer > 1) { // window titlebar
      timer = 0;
      char window_title[64] = {0};

      snprintf(
          window_title,
          sizeof(window_title),
          "Lite-Engine Demo. | %.0lf FPS | %.4f DT",
          context->time_FPS,
          context->time_delta);

      glfwSetWindowTitle(context->GLFWwindow, window_title);
    }

    { // update state
      camera_update(context);

      stars.rotation[0] = quaternion_from_euler(vector3_up(context->time_current * 0.1));
      stars_blue.rotation[0].x = stars.rotation[0].x;
      stars_blue.rotation[0].y = stars.rotation[0].y;
      stars_blue.rotation[0].z = stars.rotation[0].z;
      stars_blue.rotation[0].w = stars.rotation[0].w;

      lights[LIGHTS_POINT_0].position.x = sin(context->time_current);
      lights[LIGHTS_POINT_0].position.z = cos(context->time_current);
      lights[LIGHTS_POINT_1].position.x = cos(context->time_current);
      lights[LIGHTS_POINT_1].position.z = sin(context->time_current);
    }

    { // draw scene to the frame
      glBindFramebuffer(GL_FRAMEBUFFER, frame.FBO);

      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw_instanced(stars);
      lgl_draw_instanced(stars_blue);
    }

    { // draw the frame to the screen
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw(frame.quad);
    }

    lgl_end_frame();
  }

  lgl_object_free(stars);
  lgl_object_free(stars_blue);
  lgl_framebuffer_free(frame);
  lgl_free(context);

  return 0;
}
