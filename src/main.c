#include "gl.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "blib/b_list.h"
B_LIST_IMPLEMENTATION
DECLARE_LIST(vector3_t)
DEFINE_LIST(vector3_t)
DECLARE_LIST(matrix4_t)
DEFINE_LIST(matrix4_t)
DECLARE_LIST(quaternion_t)
DEFINE_LIST(quaternion_t)

#include "b_physics.h"
#include "blib/b_math.h"

#define ASSERT_UNIMPLEMENTED 0
	
static void error_callback(int error, const char *description) {
  (void)error;
  fprintf(stderr, "Error: %s\n", description);
  {}
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

static GLFWwindow* engine_window;
static int engine_window_size_x = 640;
static int engine_window_size_y = 480;
static int engine_window_position_x = 0;
static int engine_window_position_y = 0;
static char* engine_window_title = "Game Window";
static float engine_time_current = 0.0f;
static float engine_time_last = 0.0f;
static float engine_time_delta = 0.0f;
static float engine_renderer_FPS = 0.0f;
static vector3_t engine_ambient_light = { 0.0f, 0.0f, 0.0f };
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
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  assert(engine_window_title != NULL);

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

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //TODO API this
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(engine_window);

	engine_active_camera = (camera_t) {
      .transform.position = vector3_zero(),
      .transform.rotation = quaternion_identity(),
			.transform.scale = vector3_one(1.0),
			.projection = matrix4_identity(),
      .lookSensitivity = 10.0f,
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
	switch(engine_renderer_API) {
		case ENGINE_RENDERER_API_GL:
			glClearColor((GLfloat)r,(GLfloat)g,(GLfloat)b,(GLfloat)a);
			break;
		case ENGINE_RENDERER_API_NONE:
			break;
	}
}

static inline void transform_calculate_matrix(transform_t *t) {
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

DEFINE_LIST(cube_t)
void cube_draw(cube_t* cube) {
	if(cube->transform.scale.x < FLOAT_EPSILON &&
			cube->transform.scale.y < FLOAT_EPSILON &&
			cube->transform.scale.z < FLOAT_EPSILON) {
		//printf("saved a draw call! :D\n"); //this is why this is here...
		return;
	}
	glUseProgram(cube->material.shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube->material.diffuseMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, cube->material.specularMap);

	// model matrix
	transform_calculate_matrix(&cube->transform);

	shader_setUniformM4(cube->material.shader, "u_modelMatrix",
			&cube->transform.matrix);

	// view matrix
	shader_setUniformM4(cube->material.shader, "u_viewMatrix",
			&engine_active_camera.transform.matrix);

	// projection matrix
	shader_setUniformM4(cube->material.shader, "u_projectionMatrix", &engine_active_camera.projection);

	// camera position
	shader_setUniformV3(cube->material.shader, "u_cameraPos",
			engine_active_camera.transform.position);

	// material
	shader_setUniformInt(cube->material.shader, "u_material.diffuse", 0);
	shader_setUniformInt(cube->material.shader, "u_material.specular", 1);
	shader_setUniformFloat(cube->material.shader, "u_material.shininess", 32.0f);
	shader_setUniformV3(cube->material.shader, "u_ambientLight", engine_ambient_light);

	glBindVertexArray(cube->mesh.VAO);
	glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
}

int main(void) {
  printf("Rev up those fryers!\n");

  engine_window_title = "Game Window";
	engine_renderer_set_API(ENGINE_RENDERER_API_GL);
  engine_window_size_x = 1280;
  engine_window_size_y = 720;
  engine_window_position_x = 0;
  engine_window_position_y = 0;
  engine_start();

  engine_ambient_light = vector3_one(1.0f);
  engine_set_clear_color(0.2f, 0.3f, 0.4f, 1.0f);

	GLuint cubeShader = shader_create("res/shaders/diffuse.vs.glsl",
						                        "res/shaders/diffuse.fs.glsl");

	GLuint cubeDiffuseMap = texture_create("res/textures/container2.png");
	GLuint cubeSpecularMap = texture_create("res/textures/container2_specular.png");

	list_cube_t cubes = list_cube_t_alloc();
	for (int i = 0; i < 10; i++) {
		cube_t cube =  {
			.transform.position = vector3_back(1.0),
			.transform.rotation = quaternion_identity(),
			.mesh = mesh_alloc_cube(),
			.material = {
				.shader = cubeShader,
				.diffuseMap = cubeDiffuseMap,
				.specularMap = cubeSpecularMap,
			},
		};

		list_cube_t_add(&cubes, cube);
	}

  vector3_t look = vector3_zero();

  while (!glfwWindowShouldClose(engine_window)) {
    { // TIME
      engine_time_current = glfwGetTime();
      engine_time_delta = engine_time_current - engine_time_last;
      engine_time_last = engine_time_current;

      engine_renderer_FPS = 1 / engine_time_delta;
      // printf("============FRAME=START==============\n");
      // printf("delta %f : FPS %f\n", engine_time_delta, engine_renderer_FPS);
    } // END TIME

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
        float xangle = xoffset * engine_time_delta * engine_active_camera.lookSensitivity;
        float yangle = yoffset * engine_time_delta * engine_active_camera.lookSensitivity;

        look.x += yangle;
        look.y += xangle;

        look.y = loop(look.y, 2 * PI);
        look.x = clamp(look.x, -PI * 0.5, PI * 0.5);

        engine_active_camera.transform.rotation = quaternion_from_euler(look);
      }

      { // movement
        float cameraSpeed = 5 * engine_time_delta;
        vector3_t movement = vector3_zero();

        movement.x = glfwGetKey(engine_window, GLFW_KEY_D) -
                     glfwGetKey(engine_window, GLFW_KEY_A);
        movement.y = glfwGetKey(engine_window, GLFW_KEY_SPACE) -
                     glfwGetKey(engine_window, GLFW_KEY_LEFT_SHIFT);
        movement.z = glfwGetKey(engine_window, GLFW_KEY_W) -
                     glfwGetKey(engine_window, GLFW_KEY_S);

        movement = vector3_normalize(movement);
        movement = vector3_scale(movement, cameraSpeed);
        movement = vector3_rotate(movement, engine_active_camera.transform.rotation);

        engine_active_camera.transform.position =
            vector3_add(engine_active_camera.transform.position, movement);
      }
    } // END INPUT

		//projection
    glfwGetWindowSize(engine_window, &engine_window_size_x,
                      &engine_window_size_y);
    float aspect = (float)engine_window_size_x / (float)engine_window_size_y;
    engine_active_camera.projection = matrix4_perspective(deg2rad(90), aspect, 0.1f, 1000.0f);

    { // draw
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      transform_calculate_matrix(&engine_active_camera.transform);

			for (size_t i = 0; i < cubes.length; i++) {
				float scale = fabs(sinf(engine_time_current));
				cubes.data[i].transform.scale = vector3_one(scale);
				cube_draw(&cubes.data[i]);
			}

      glfwSwapBuffers(engine_window);
      glfwPollEvents();
    }
  }

	list_cube_t_free(&cubes);

  glfwTerminate();
  return 0;
}
