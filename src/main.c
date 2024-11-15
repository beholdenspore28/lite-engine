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

#define DEBUG_LOG_TIME 1
#define USE_DEPRECATED_TIME 0

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
static int         engine_window_size_x        = 640;
static int         engine_window_size_y        = 480;
static int         engine_window_position_x    = 0;
static int         engine_window_position_y    = 0;
static bool        engine_window_fullscreen    = false;
static char*       engine_window_title         = "Game Window";
static bool        engine_window_always_on_top = false;
static float        engine_time_current         = 0.0f;
static float        engine_time_last            = 0.0f;
static float        engine_time_delta           = 0.0f;
static uint64_t    engine_frame_current        = 0;
static float        engine_renderer_FPS         = 0.0f;
static vector3_t   engine_ambient_light        = { 0.1, 0.1, 0.1 };
static camera_t    engine_active_camera        = { 0 };

typedef uint32_t EntityId;
static EntityId entity_count;
enum { ENTITY_NULL = 0, MAX_ENTITIES = 8 };

EntityId entity_register(void) {
	assert(entity_count < MAX_ENTITIES);
	entity_count++;
	return entity_count;
}

typedef struct {
	float constant;
	float linear;
	float quadratic;
	vector3_t diffuse;
	vector3_t specular;
} pointLight_t;

EntityId light;

typedef struct {
	mesh_t *mesh;
	GLuint *shader;
	transform_t *transform;
	material_t *material;
	pointLight_t *pointlight;
} component_registry;

component_registry *component_registry_alloc(void) {
	component_registry *r = malloc(sizeof(component_registry));
	r->mesh = calloc(sizeof(mesh_t), MAX_ENTITIES);
	r->shader = calloc(sizeof(GLuint), MAX_ENTITIES);
	r->transform = calloc(sizeof(transform_t), MAX_ENTITIES);
	r->material = calloc(sizeof(material_t), MAX_ENTITIES);
	r->pointlight = calloc(sizeof(pointLight_t), MAX_ENTITIES);
	return r;
}

void component_registry_free(component_registry *r) {
	free(r->mesh);
	free(r->shader);
	free(r->transform);
	free(r->material);
	free(r->pointlight);
	free(r);
}

// set window resolution
void engine_window_set_resolution(int x, int y) {
	glfwSetWindowSize(engine_window, x, y);
}

// position window in the center of the screen
void engine_window_set_position(int x, int y) {
	glfwSetWindowPos(engine_window, x, y);
}


void engine_start(void) {
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

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	int width, height;
	glfwGetFramebufferSize(engine_window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSwapInterval(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(engine_window);

	engine_active_camera = (camera_t){
		.transform.position = (vector3_t){0.0, 0.0, -20.0},
			.transform.rotation = quaternion_identity(),
			.transform.scale = vector3_one(1.0),
			.projection = matrix4_identity(),
			.lookSensitivity = 0.002f,
	};
}

void engine_set_clear_color(float r, float g, float b, float a) {
	glClearColor((GLfloat)r, (GLfloat)g, (GLfloat)b, (GLfloat)a);
}

void mesh_draw(component_registry *r, EntityId e) {
	glUseProgram(r->shader[e]);

	// model matrix
	transform_calculate_matrix(&r->transform[e]);

	shader_setUniformM4(r->shader[e], "u_modelMatrix", &r->transform[e].matrix);

	// view matrix
	shader_setUniformM4(r->shader[e], "u_viewMatrix",
			&engine_active_camera.transform.matrix);

	// projection matrix
	shader_setUniformM4(r->shader[e], "u_projectionMatrix",
			&engine_active_camera.projection);

	// material
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, r->material[e].diffuseMap);
	vector3_t color = {1.0f, 0.0f, 0.0f};
	shader_setUniformV3(r->shader[e], "u_color", color);
	shader_setUniformInt(r->shader[e], "u_texture", 0);

	glBindVertexArray(r->mesh[e].VAO);
	glDrawElements(GL_TRIANGLES, r->mesh[e].indexCount, GL_UNSIGNED_INT, 0);
}

#if 0
mesh_t mesh_alloc_planet(const int subdivisions, const float radius) {
	list_vertex_t vertices = list_vertex_t_alloc();
	list_uint32_t indices = list_uint32_t_alloc();
	int index = 0;
	enum { FACE_FRONT, FACE_BACK, FACE_RIGHT, FACE_LEFT, FACE_UP, FACE_DOWN };
	for (int faceDirection = 0; faceDirection < 6; faceDirection++) {
		int offset = subdivisions * subdivisions * faceDirection;
		for (int k = 0; k < subdivisions; k++) {
			for (int j = 0; j < subdivisions; j++) {

				// calculate vertex position on unit cube's surface
				vector3_t point = {0};
				point.y = map(k, 0, subdivisions - 1, -0.5, 0.5);
				point.x = map(j, 0, subdivisions - 1, -0.5, 0.5);
				point.z = 0.5;

				// calculate indices
				if (k < subdivisions - 1 && j < subdivisions - 1) {
					int first = offset + (k * (subdivisions)) + j;
					int second = first + subdivisions;
					for (int i = 0; i < 6; i++) {
						list_uint32_t_add(&indices, 0);
					}
					// one quad face
					indices.data[index++] = first; // triangle 1
					indices.data[index++] = second;
					indices.data[index++] = first + 1;
					indices.data[index++] = second; // triangle 2
					indices.data[index++] = second + 1;
					indices.data[index++] = first + 1;
				}

				{ // calculate face direction
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
				}

				// sphere-ify
				vector3_t pointOnSphere = vector3_normalize(point);

				// noise
				float freq = 1, 
					  amp = 0.4, 
					  offset = 10,
					  wave = noise3_fbm(
							  pointOnSphere.x * freq + offset,
							  pointOnSphere.y * freq + offset,
							  pointOnSphere.z * freq + offset) * amp;

				// texture coordinates
				float tu = map(k, 0, subdivisions - 1, 0, 1),
					  tv = map(j, 0, subdivisions - 1, 0, 1);

				// add vertex
				vertex_t vertex = {
					.position = vector3_scale(pointOnSphere, wave + radius),
					.normal = pointOnSphere,
					.texCoord = {tu, tv},
				};
				list_vertex_t_add(&vertices, vertex);
			}
		}
	}
	mesh_t m = mesh_alloc(&vertices.data[0], &indices.data[0], vertices.length,
			indices.length);
	list_vertex_t_free(&vertices);
	list_uint32_t_free(&indices);
	return m;
}
#else
mesh_t mesh_alloc_planet(const int resolution, const float radius) {
	list_vertex_t vertices = list_vertex_t_alloc();
	list_uint32_t indices = list_uint32_t_alloc();
	
	enum { FACE_FRONT, FACE_BACK, FACE_RIGHT, FACE_LEFT, FACE_UP, FACE_DOWN };
	
	int index = 0;
	for(int face = 0; face < 6; face++) {
		for(int quad = 0; quad < 4; quad++) {
			for (int k = 0; k < resolution; k++) {
				for (int j = 0; j < resolution; j++) {
					int offset = resolution*resolution*quad*face;
				
					vector3_t point = {0};
					
					if (quad == 0) {
						point.x = map(j, 0, resolution - 1, -0.25, 0.25) - 0.25;
						point.y = map(k, 0, resolution - 1, -0.25, 0.25) - 0.25;
					}else if (quad == 1) {
						point.x = map(j, 0, resolution - 1, -0.25, 0.25) - 0.25;
						point.y = map(k, 0, resolution - 1, -0.25, 0.25) + 0.25;
					}else if (quad == 2) {
						point.x = map(j, 0, resolution - 1, -0.25, 0.25) + 0.25;
						point.y = map(k, 0, resolution - 1, -0.25, 0.25) - 0.25;
					}else if (quad == 3) {
						point.x = map(j, 0, resolution - 1, -0.25, 0.25) + 0.25;
						point.y = map(k, 0, resolution - 1, -0.25, 0.25) + 0.25;
					}
					point.z = 0.5;
					
					{ // calculate face direction
						vector3_t original = point;
						switch (face) {
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
					}
					if (k < resolution - 1 && j < resolution - 1) {
						int first = offset + (k * (resolution)) + j;
						int second = first + resolution;
						for (int i = 0; i < 6; i++) {
							list_uint32_t_add(&indices, 0);
						}
						indices.data[index++] = first;
						indices.data[index++] = second;
						indices.data[index++] = first    + 1;
	
						indices.data[index++] = second;
						indices.data[index++] = second  + 1;
						indices.data[index++] = first    + 1;
					}
	
					vector3_t pointOnSphere = vector3_normalize(point);
	
					float freq = 1, 
						amp = 0.4, 
						noiseOffset = 10,
						wave = noise3_fbm(
								pointOnSphere.x * freq + noiseOffset,
								pointOnSphere.y * freq + noiseOffset,
								pointOnSphere.z * freq + noiseOffset) * amp;
	
					float tx = map(k, 0, resolution - 1, 0, 1),
						ty = map(j, 0, resolution - 1, 0, 1);
	
					vertex_t vertex = {
						//~ .position = vector3_scale(pointOnSphere, radius*0.5),
						.position = vector3_scale(point, radius*0.5),
						.normal = pointOnSphere,
						.texCoord = {tx, ty},
					};
					list_vertex_t_add(&vertices, vertex);
				}
			}
		}
	}

	mesh_t m = mesh_alloc(vertices.data, indices.data, vertices.length,
			indices.length);
	list_vertex_t_free(&vertices);
	list_uint32_t_free(&indices);
	return m;
}
#endif
mesh_t mesh_alloc_cube_sphere(const int subdivisions, const float radius) {
	list_vertex_t vertices = list_vertex_t_alloc();
	list_uint32_t indices = list_uint32_t_alloc();
	int index = 0;
	for (int faceDirection = 0; faceDirection < 6; faceDirection++) {
		int offset = subdivisions * subdivisions * faceDirection;
		for (int k = 0; k < subdivisions; k++) {
			for (int j = 0; j < subdivisions; j++) {

				vector3_t point = {
					map(k, 0, subdivisions - 1, -1, 1),
					map(j, 0, subdivisions - 1, -1, 1),
					1,
				};

				if (k < subdivisions - 1 && j < subdivisions - 1) {
					int first = offset + (k * (subdivisions)) + j;
					int second = first + subdivisions;
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

				float u = map(k, 0, subdivisions - 1, 0, 1),
					  v = map(j, 0, subdivisions - 1, 0, 1);

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
	list_vertex_t_free(&vertices);
	list_uint32_t_free(&indices);
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
	list_vertex_t_free(&vertices);
	list_uint32_t_free(&indices);
	return m;
}

int main(void) {
	printf("Rev up those fryers!\n");

	engine_window_title = "Game Window";
	engine_window_size_x = 1280;
	engine_window_size_y = 720;
	engine_window_position_x = 1280 / 2;
	engine_window_position_y = 0;
	engine_start();
	engine_set_clear_color(0.0, 0.0, 0.0, 1.0);

	component_registry *registry = component_registry_alloc();

	// shader creation.
	//~ GLuint diffuseShader = shader_create("res/shaders/diffuse.vs.glsl",
			//~ "res/shaders/diffuse.fs.glsl");

	GLuint unlitShader =
		shader_create("res/shaders/unlit.vs.glsl", "res/shaders/unlit.fs.glsl");

	GLuint testDiffuseMap = texture_create("res/textures/test.png");
	GLuint testSpecularMap = texture_create("res/textures/test.png");

	// skybox creation
	EntityId skybox = entity_register();
	registry->mesh[skybox] = mesh_alloc_cube();
	registry->shader[skybox] = unlitShader;
	registry->material[skybox] = (material_t){
		.diffuseMap = texture_create("res/textures/space.png"),
			.specularMap = testSpecularMap,
	};
	registry->transform[skybox] = (transform_t){
		.position = {0},
			.rotation = quaternion_identity(),
			.scale = vector3_one(10.0),
	};

	// cube creation
	EntityId cube = entity_register();
	registry->mesh[cube] = mesh_alloc_cube();
	registry->shader[cube] = unlitShader;
	registry->material[cube] = (material_t){
		.diffuseMap = testDiffuseMap,
			.specularMap = testSpecularMap,
	};
	registry->transform[cube] = (transform_t){
		.position = {-10, 0, 0},
			.rotation = quaternion_identity(),
			.scale = vector3_one(1.0),
	};

	// create planet
	EntityId planet = entity_register();
	registry->mesh[planet] = mesh_alloc_planet(4, 1);
	registry->shader[planet] = unlitShader;
	registry->material[planet] = (material_t){
		.diffuseMap = texture_create("res/textures/lunarrock_d.png"),
			.specularMap = testSpecularMap,
	};
	registry->transform[planet] = (transform_t){
		.position = {1, 0, 150},
			.rotation = quaternion_identity(),
			.scale = vector3_one(100.0),
	};

	// create light
	light = entity_register();
	registry->pointlight[light] = (pointLight_t){
		.diffuse = vector3_one(0.8f),
			.specular = vector3_one(1.0f),
			.constant = 1.0f,
			.linear = 0.09f,
			.quadratic = 0.032f,
	};
	registry->transform[light].position = (vector3_t){5, 5, 5};
	vector3_t mouseLookVector = vector3_zero();

	while (!glfwWindowShouldClose(engine_window)) {
		{ // update time
			engine_time_current = glfwGetTime();
			engine_time_delta   = engine_time_current - engine_time_last;
			engine_time_last    = engine_time_current;

			engine_renderer_FPS = 1 / engine_time_delta;
			engine_frame_current++;

#if DEBUG_LOG_TIME // log time
			printf("time_current  %f\n"
					"time_last     %f\n"
					"time_delta    %f\n"
					"FPS           %f\n"
					"frame_current %lu\n", 
					engine_time_current, 
					engine_time_last,
					engine_time_delta,
					engine_renderer_FPS,
					engine_frame_current);
#endif // log time
		}

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
				float cameraSpeed = 32 * engine_time_delta;
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
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				} else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
			registry->transform[skybox].rotation =
				quaternion_conjugate(engine_active_camera.transform.rotation);
			mesh_draw(registry, skybox);
			glEnable(GL_DEPTH_TEST);

			transform_calculate_view_matrix(&engine_active_camera.transform);

			mesh_draw(registry, planet);
			mesh_draw(registry, cube);

			glfwSwapBuffers(engine_window);
			glfwPollEvents();
		}
	}
	
	component_registry_free(registry);
	glfwTerminate();
	return 0;
}
