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

void objects_animate(
    const size_t             count,
          lgl_render_data_t *objects,
    const lgl_context_t     *context) {

  for(size_t i = 0; i < count; i++) {
      objects[i].position.y = cos(context->time_current) * 0.2 + 0.5;
      objects[i].position = context->camera.position;
      objects[i].rotation = quaternion_multiply(
          objects[i].rotation,
          quaternion_from_euler((vector3_t) { 0, context->time_delta, 0 }));
  }
}

void camera_update(lgl_context_t *context) {

  { // movement
    vector3_t movement = vector3_zero(); {

      // SPACE SHIFT
      movement.y += glfwGetKey(context->GLFWwindow, GLFW_KEY_SPACE) -
        glfwGetKey(context->GLFWwindow, GLFW_KEY_LEFT_SHIFT);

      // WASD
      movement.x += glfwGetKey(context->GLFWwindow, GLFW_KEY_D) -
        glfwGetKey(context->GLFWwindow, GLFW_KEY_A);

      movement.z += glfwGetKey(context->GLFWwindow, GLFW_KEY_W) -
        glfwGetKey(context->GLFWwindow, GLFW_KEY_S);

      // ARROW KEYS
      movement.x += glfwGetKey(context->GLFWwindow, GLFW_KEY_RIGHT) -
        glfwGetKey(context->GLFWwindow, GLFW_KEY_LEFT);

      movement.z += glfwGetKey(context->GLFWwindow, GLFW_KEY_UP) -
        glfwGetKey(context->GLFWwindow, GLFW_KEY_DOWN);
    }

    movement = vector3_normalize(movement);
    movement = vector3_scale(movement, context->time_delta *10);
    movement = vector3_rotate(movement, context->camera.rotation);

    context->camera.position = vector3_add(context->camera.position, movement);
  }

  { // mouse look
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

#if 0
// distribute the objects randomly inside a box
static inline void box_distribution(
    unsigned int       count,
    lgl_render_data_t *objects,
    float              length,
    int                seed) {
  for(unsigned int i = 0; i < count; i++) {

    objects[i].position.x = noise3( seed + i + 1, seed + i,     seed + i     );
    objects[i].position.y = noise3( seed + i,     seed + i + 1, seed + i     );
    objects[i].position.z = noise3( seed + i,     seed + i,     seed + i + 1 );

    objects[i].position = vector3_scale(objects[i].position, length);

    objects[i].position.x -= length * 0.5;
    objects[i].position.y -= length * 0.5;
    objects[i].position.z -= length * 0.5;
  }
}
#endif

static inline void sphere_distribution(
    unsigned int       count,
    lgl_render_data_t *objects,
    float              radius,
    int                seed) {
  for(unsigned int i = 0; i < count; i++) {

    vector3_t eulerAngles = vector3_zero();

    eulerAngles.x = noise3( seed + i  , seed + i,   seed + i   ) * 2 * PI;
    eulerAngles.y = noise3( seed + i+1, seed + i+1, seed + i+1 ) * 2 * PI;
    eulerAngles.z = noise3( seed + i+2, seed + i+2, seed + i+2 ) * 2 * PI;

    objects[i].position = vector3_rotate(vector3_forward(noise1(i)*radius), quaternion_from_euler(eulerAngles));
  }
}

int main() {
  lgl_context_t *context = lgl_start(854, 480);

  // ---------------------------------------------------------------
  // Create shaders
  
  GLuint shader_phong = 0; {
    GLuint vertex_shader = lgl_shader_compile(
        "res/shaders/phong_vertex.glsl",
        GL_VERTEX_SHADER);

    GLuint fragment_shader = lgl_shader_compile(
        "res/shaders/phong_fragment.glsl",
        GL_FRAGMENT_SHADER);

    shader_phong = lgl_shader_link(vertex_shader, fragment_shader);
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
  // Create textures

  GLuint texture_cube     = lgl_texture_alloc("res/textures/lite-engine-cube.png");

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

  enum { STARS_LENGTH = 256 };
  lgl_render_data_t star = lgl_cube_alloc();
  lgl_render_data_t stars[STARS_LENGTH];

  for(int i = 0; i < STARS_LENGTH; i++) {
    stars[i] = star;
    stars[i].shader         = shader_phong;
    stars[i].diffuse_map    = texture_cube;
    stars[i].lights_count   = LIGHTS_COUNT;
    stars[i].lights         = lights;
    stars[i].scale          = vector3_one(1);
  }

  sphere_distribution(STARS_LENGTH, stars, 50, 1);

  lgl_render_data_t asteroid = asteroid_mesh_alloc(); {
    asteroid.shader         =  shader_phong;
    asteroid.diffuse_map    =  texture_cube;
    asteroid.position.z     =  1;
    asteroid.lights_count   =  LIGHTS_COUNT;
    asteroid.lights         =  lights;
  }

  // ---------------------------------------------------------------
  // Create framebuffer
  
  lgl_framebuffer_t frame = lgl_framebuffer_alloc(shader_framebuffer);
  lgl_active_framebuffer_set(&frame);

  // ---------------------------------------------------------------
  // Create camera

  context->camera.rotation = quaternion_identity();
  context->camera.position = vector3_zero();
  context->camera.position.z = -100;

  // ---------------------------------------------------------------
  // game loop
  
  glClearColor(0,0,0,1);
  while(!glfwWindowShouldClose(context->GLFWwindow)) {

    { // update state
      camera_update(context);
      //objects_animate(1, &stars, context);

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

      lgl_draw(STARS_LENGTH, stars);
    }

    { // draw the frame to the screen
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glClear(
          GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT |
          GL_STENCIL_BUFFER_BIT);

      lgl_draw(1, &frame.quad);
    }

    lgl_end_frame(context);
  }

  lgl_free(context);

  return 0;
}
