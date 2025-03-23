#include "lgl.h"
#define BLIB_IMPLEMENTATION
#include "blib/blib_math3d.h"

lgl_render_data_t asteroid_mesh_alloc(void) {

	lgl_render_data_t mesh;
  mesh.vertices = list_lgl_vertex_t_alloc();

	const float   radius     = 0.1;
	const float   resolution = 10.0;

	for(int face = 0; face < 6; face++) {
		for(float x = 0; x < resolution; x++) {
			for(float y = 0; y < resolution; y++) {

				vector3_t position = (vector3_t) { 
					.x = map(x, 0, resolution -1, -0.5, 0.5), 
					.y = map(y, 0, resolution -1, -0.5, 0.5), 
					.z = 0.5,
				};
				position = vector3_normalize(position);

				const vector3_t temp = position;
				switch (face) {
					case 0: { // 0 is front
                    // do nothing.
						position.x = -temp.x;
					} break;
					case 1: { // 1 is rear
						position.z = -position.z;
					}break;
					case 2: { // 2 is left
						position.z = temp.x;
						position.x = temp.z;
					} break;	
					case 3: { // 3 is right
						position.z = -temp.x;
						position.x = -temp.z;
					} break;
					case 4: { // 4 is bottom
						position.y = -temp.z;
						position.z = -temp.y;
					} break;
					case 5: { // 5 is top
						position.y = temp.z;
						position.z = temp.y;
					} break;
					default: {
						fprintf(stderr, "rock generator encountered invalid face index");
					} break;
				}

				const float amplitude = 5.0;
				const float frequency = 1;
				const float noise = radius + (noise3_fbm(
							position.x * frequency,
							position.y * frequency,
							position.z * frequency) * amplitude);

				lgl_vertex_t v = (lgl_vertex_t) {
					.position = vector3_scale(position, noise),
					.normal = position,
					.texture_coordinates = vector2_zero(),
				};

				list_lgl_vertex_t_add(&mesh.vertices, v);
			}
		}
	}

	return mesh;
}

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

lgl_mat4_print(GLfloat *mat) {
    debug_log("Matrix");
    for(int j = 0; j < 4; j++) {
      printf("%f ", mat[j]);
    }
    putchar('\n');
    for(int j = 4; j < 8; j++) {
      printf("%f ", mat[j]);
    }
    putchar('\n');
    for(int j = 8; j < 12; j++) {
      printf("%f ", mat[j]);
    }
    putchar('\n');
    for(int j = 12; j < 16; j++) {
      printf("%f ", mat[j]);
    }
    putchar('\n');

}
// distribute the objects randomly inside a box
static inline vector3_t vector3_point_in_unit_cube(unsigned int seed) {
  vector3_t ret = vector3_zero();
  ret.x = noise3(seed + 1, seed,     seed     ) * 2.0 - 1.0;
  ret.y = noise3(seed,     seed + 1, seed     ) * 2.0 - 1.0;
  ret.z = noise3(seed,     seed,     seed + 1 ) * 2.0 - 1.0;
  return ret;
}

vector3_t vector3_point_in_unit_sphere(unsigned int seed) {
    float d, x, y, z;
    int i = 0;
    do {
        x = noise3( seed + i + 1, seed + i,     seed + i     ) * 2.0 - 1.0;
        y = noise3( seed + i,     seed + i + 1, seed + i     ) * 2.0 - 1.0;
        z = noise3( seed + i,     seed + i,     seed + i + 1 ) * 2.0 - 1.0;
        d = x*x + y*y + z*z;
        i++;
    } while(d > 1.0);
    return (vector3_t) { x, y, z };
}

static inline vector3_t swirl(vector3_t point, float strength_01) {
  float swirl_amount = vector3_square_magnitude(point) * strength_01;
  return vector3_rotate(point, quaternion_from_euler(vector3_up(swirl_amount)));
}

void galaxy_distribution(
    unsigned int       count,
    lgl_render_data_t *stars,
    float              radius,
    float              swirl_strength,
    float              arm_thickness,
    float              arm_length,
    float              tilt) {

  for(unsigned int i = 0; i < count; i++) {
    // two tone color
    if (i % 2 == 0) {
      stars[i].color = (vector4_t) { 0.5, 0.5, 1.0, 1.0 };
    } else {
      stars[i].color = (vector4_t) { 1.0, 1.0, 1.0, 1.0 };
    }

    stars[i].position = vector3_point_in_unit_sphere(i);
    vector3_t gravity = vector3_normalize(vector3_negate(stars[i].position));
    stars[i].position = vector3_add(stars[i].position, gravity);

    stars[i].position.x *= arm_thickness;
    stars[i].position.z *= arm_length;
    
    stars[i].position = swirl(stars[i].position, swirl_strength);

    stars[i].position = vector3_scale(stars[i].position, radius);

    stars[i].position = vector3_rotate(
        stars[i].position, quaternion_from_euler(vector3_right(tilt)));
  }
}

int main() {
  lgl_context_t *context = lgl_start(854, 480);

  // ---------------------------------------------------------------
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

  // ---------------------------------------------------------------
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

  // ---------------------------------------------------------------
  // Create stars

  enum { STARS_LENGTH = 10 };
  lgl_render_data_t star = lgl_cube_alloc();
  lgl_render_data_t stars[STARS_LENGTH];

  for(int i = 0; i < STARS_LENGTH; i++) {
    stars[i]                = star;
    stars[i].shader         = shader_solid;
    stars[i].scale          = vector3_one(0.1);
  }

  galaxy_distribution(STARS_LENGTH, stars, 10, 0.1, 4, 10, PI/5);

  GLfloat model_matrices[STARS_LENGTH][16];
  for(unsigned int i = 0; i < STARS_LENGTH; i++) {

    lgl_mat4_identity(model_matrices[i]);

    {
      GLfloat scale[16]; lgl_mat4_identity(scale);
      scale[0 ] = stars[i].scale.x;
      scale[5 ] = stars[i].scale.y;
      scale[10] = stars[i].scale.z;

      GLfloat translation[16]; lgl_mat4_identity(translation);
      translation[12] = stars[i].position.x;
      translation[13] = stars[i].position.y;
      translation[14] = stars[i].position.z;

      GLfloat rotation[16] = {0};
      quaternion_to_mat4(stars[i].rotation, rotation);

      lgl_mat4_multiply(model_matrices[i], scale, rotation);
      lgl_mat4_multiply(model_matrices[i], model_matrices[i], translation);
    }
  }

  for(int i = 0; i < STARS_LENGTH; i++) {
    lgl_mat4_print(model_matrices[i]);
  }

  // -------------------------
  // configure instanced array

  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  glBufferData(
      GL_ARRAY_BUFFER, STARS_LENGTH * sizeof(GLfloat) * 16,
      model_matrices, GL_STATIC_DRAW);

  unsigned int VAO = stars[0].VAO;
  glBindVertexArray(VAO);

  // set attribute pointers for matrix (4 times vec4)
  glVertexAttribPointer(
      3, 4, GL_FLOAT, GL_FALSE,
      sizeof(GLfloat)*16, (void*)0);

  glVertexAttribPointer(
      4, 4, GL_FLOAT, GL_FALSE,
      sizeof(GLfloat)*16, (void*)(sizeof(vector4_t)));

  glVertexAttribPointer(
      5, 4, GL_FLOAT, GL_FALSE,
      sizeof(GLfloat)*16, (void*)(2 * sizeof(vector4_t)));

  glVertexAttribPointer(
      6, 4, GL_FLOAT, GL_FALSE,
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

  // ---------------------------------------------------------------
  // Create framebuffer
  
  lgl_framebuffer_t frame = lgl_framebuffer_alloc(shader_framebuffer);
  lgl_active_framebuffer_set(&frame);

  // ---------------------------------------------------------------
  // Create camera

  context->camera.rotation = quaternion_identity();
  context->camera.position = vector3_zero();
  context->camera.position.z = -50;

  GLfloat view[16]; lgl_mat4_identity(view);
  GLfloat projection[16]; lgl_mat4_identity(projection);

  context->camera.view = view;
  context->camera.projection = projection;

  // ---------------------------------------------------------------
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

      // speen
      for(size_t i = 0; i < STARS_LENGTH; i++) {
        stars[i].position = vector3_rotate(
            stars[i].position,
            quaternion_from_euler(vector3_up(context->time_delta * 0.1)));
      }

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

#if 0
      lgl_draw(STARS_LENGTH, stars);
#else
      lgl_draw_instanced(STARS_LENGTH, stars);
#endif
    }

    { // draw the frame to the screen
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw(1, &frame.quad);
    }

    lgl_end_frame();
  }

  lgl_free(context);

  return 0;
}
