#include "gl.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "blib/blib.h"
#include "blib/bmath.h"
#include "physics.h"

B_LIST_IMPLEMENTATION
DECLARE_LIST(vector3_t)
DEFINE_LIST(vector3_t)
DECLARE_LIST(matrix4_t)
DEFINE_LIST(matrix4_t)
DECLARE_LIST(quaternion_t)
DEFINE_LIST(quaternion_t)

#define ASSERT_UNIMPLEMENTED 0

	//debug toggles
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

static GLFWwindow* engine_window;
static int engine_window_size_x = 640;
static int engine_window_size_y = 480;
static int engine_window_position_x = 0;
static int engine_window_position_y = 0;
static bool engine_window_fullscreen = false;
static char* engine_window_title = "Game Window";
static bool engine_window_always_on_top = false;
static float engine_time_current = 0.0f;
static float engine_time_last = 0.0f;
static float engine_time_delta = 0.0f;
static ui64 engine_time_current_frame = 0;
static float engine_renderer_FPS = 0.0f;
static vector3_t engine_ambient_light = {0.3, 0.3, 0.3};
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

	if (engine_window_always_on_top)
		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

	assert(engine_window_title != NULL);

	if (engine_window_fullscreen)
		engine_window = glfwCreateWindow(engine_window_size_x, engine_window_size_y,
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

	engine_active_camera = (camera_t) {
		.transform.position = (vector3_t){0.0, 0.0, -30.0},
			.transform.rotation = quaternion_identity(),
			.transform.scale = vector3_one(1.0),
			.projection = matrix4_identity(),
			.lookSensitivity = 0.01f,
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

	ui32 drawCallsSaved = 0;

	typedef struct {
		vector3_t position;
		float constant;
		float linear;
		float quadratic;
		vector3_t diffuse;
		vector3_t specular;
	} pointLight_t;

pointLight_t light;

void quad_draw(quad_t* quad) {
	if(quad->transform.scale.x < FLOAT_EPSILON &&
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
	vector3_t color = { 1.0f, 0.0f, 0.0f };
	shader_setUniformV3(quad->material.shader, "u_color", color);
	shader_setUniformInt(quad->material.shader, "u_texture", 0);


	glBindVertexArray(quad->mesh.VAO);
	glDrawElements(GL_TRIANGLES, MESH_QUAD_NUM_INDICES, GL_UNSIGNED_INT, 0);
}

void sphere_draw(primitive_shape_t* sphere) {
	if(sphere->transform.scale.x < FLOAT_EPSILON &&
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
	shader_setUniformFloat(sphere->material.shader, "u_material.shininess", 32.0f);
	shader_setUniformV3(sphere->material.shader, "u_ambientLight", engine_ambient_light);

	glBindVertexArray(sphere->mesh.VAO);
	glDrawElements(GL_TRIANGLES, 1000, GL_UNSIGNED_INT, 0);
}

void cube_draw(primitive_shape_t* cube) {
	if(cube->transform.scale.x < FLOAT_EPSILON &&
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
	shader_setUniformV3(cube->material.shader, "u_ambientLight", engine_ambient_light);

	glBindVertexArray(cube->mesh.VAO);
	glDrawElements(GL_TRIANGLES, MESH_CUBE_NUM_INDICES, GL_UNSIGNED_INT, 0);
}

DECLARE_LIST(vertex_t)
	DEFINE_LIST(vertex_t)

int main(void) {
	printf("Rev up those fryers!\n");

	engine_window_title = "Game Window";
	engine_renderer_set_API(ENGINE_RENDERER_API_GL);
	engine_window_size_x = 640;
	engine_window_size_y = 480;
	engine_window_position_x = 850;
	engine_window_position_y = 150;
	//engine_window_fullscreen = true;
	//engine_window_always_on_top = true;
	engine_start();

	engine_set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);

	// shader creation.
	GLuint diffuseShader = shader_create("res/shaders/diffuse.vs.glsl",
			"res/shaders/diffuse.fs.glsl");
	GLuint unlitShader = shader_create("res/shaders/unlit.vs.glsl",
			"res/shaders/unlit.fs.glsl");

	// skybox creation
	GLuint skyboxDiffuseMap = texture_create("res/textures/stars.jpg");

	primitive_shape_t skybox = {
		.transform.position = (vector3_t) { 0.0f, 0.0f, 0.0f },
		.transform.rotation = quaternion_identity(),
		.transform.scale = vector3_one(1000.0),
		.mesh = mesh_alloc_cube(true),
		.material = {
			.shader = unlitShader,
			.diffuseMap = skyboxDiffuseMap,
			.specularMap = 0,
		},
	};

	GLuint cubeDiffuseMap = texture_create("res/textures/container2.png");
	GLuint cubeSpecularMap = texture_create("res/textures/container2_specular.png");

	enum {
		total = 18,
		radius = 20,
		iBufferSize = 10000,
	};

	// create sphere's vertices
	list_vertex_t vertices = list_vertex_t_alloc();
	for (int i = 0; i < total*0.5+1; i++) {
		float lon = map(i, 0, total, -PI, PI);
		for (int j = 0; j < total; j++) {
			float lat = map(j, 0, total, -PI, PI);
			float x = radius * sin(lon) * cos(lat);
			float y = radius * sin(lon) * sin(lat);
			float z = radius * cos(lon);
			vertex_t newVertex = {
				.position = (vector3_t){x,y,z},
			};
			list_vertex_t_add(&vertices, newVertex);
			if (i == 0) j = total;
			if (i == total*0.5) j = total;
		}
	}

	// create triangles at the south pole
	GLuint indices[iBufferSize] = {0,1,2};
	int i;
	for (i = 3; i < total*3; i+=3) {
		indices[i]   = 0;
		indices[i+1] = indices[i-2]+1;
		indices[i+2] = indices[i-1]+1;
	}
	indices[i-1] = indices[1];

	// create quads 
	int quad = 0;
	for (i = i; i < total*3+total*6*7; i+=6, quad++) {
		if (quad % total-total+1 == 0) {
			// this is where you patch the start to the end
			continue; 
		}	
		indices[i]   = quad + total - (total - 1);
		indices[i+1] = quad + total + 1;
		indices[i+2] = quad + total + 2;
		indices[i+3] = quad + total - (total - 1);
		indices[i+4] = quad + total + 2;
		indices[i+5] = quad + total - (total - 2);
	}

	// create a cube on each of the sphere's vertices
	list_primitive_shape_t cubes = list_primitive_shape_t_alloc();
	for (size_t i = 0; i < vertices.length; i++) {
			primitive_shape_t cube = {
				.transform.position = vertices.data[i].position,
				.transform.rotation = quaternion_from_euler((vector3_t){0,i,0}),
				.transform.scale    = vector3_one(0.2),
				.mesh = mesh_alloc_cube(false),
				.material = {
					.shader = diffuseShader,
					.diffuseMap = cubeDiffuseMap,
					.specularMap = cubeSpecularMap
				},
			};
			list_primitive_shape_t_add(&cubes, cube);
	}
	
	cubes.data[total].transform.scale = vector3_one(0.4);
	cubes.data[total+total].transform.scale = vector3_one(0.5);
	cubes.data[1].transform.scale = vector3_one(0.6);
	cubes.data[total+1].transform.scale = vector3_one(0.7);

	// create sphere using indices and vertices
	primitive_shape_t sphere = {
		.transform.position = vector3_zero(),
		.transform.rotation = quaternion_identity(),
		.transform.scale = vector3_one(1.0),
		.mesh = mesh_alloc(&vertices.data[0], indices, vertices.length, iBufferSize),
		.material = {
			.shader = diffuseShader,
			.diffuseMap = cubeDiffuseMap,
			.specularMap = cubeSpecularMap,
		},
	};

	// create point light
	light = (pointLight_t) {
		.position =	(vector3_t){ 0.0f,0.0f,2.0f},
			.diffuse =	vector3_one(0.8f),
			.specular =	vector3_one(1.0f),
			.constant =	1.0f,
			.linear =	0.09f,
			.quadratic =	0.032f,
	};

	vector3_t mouseLookVector = vector3_zero();
	int useWireframeMode = false;

	while (!glfwWindowShouldClose(engine_window)) {
		{ // TIME
			engine_time_current = glfwGetTime();
			engine_time_delta = engine_time_current - engine_time_last;
			engine_time_last = engine_time_current;

			engine_renderer_FPS = 1 / engine_time_delta;
			engine_time_current_frame++;
#if ENGINE_SHOW_STATS_TIME
			printf("============FRAME=START==============\n");
			printf("DELTA_TIME: %f | FPS: %f | CURRENT_FRAME %d\n", engine_time_delta, engine_renderer_FPS, engine_time_current_frame);
#endif // ENGINE_SHOW_STATS_TIME
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

				mouseLookVector.x += yoffset * engine_active_camera.lookSensitivity;
				mouseLookVector.y += xoffset * engine_active_camera.lookSensitivity;

				mouseLookVector.y = loop(mouseLookVector.y, 2 * PI);
				mouseLookVector.x = clamp(mouseLookVector.x, -PI * 0.5, PI * 0.5);

				vector3_scale(mouseLookVector, engine_time_delta);

				engine_active_camera.transform.rotation = quaternion_from_euler(mouseLookVector);
			}

			{ // movement
				float cameraSpeed = 15 * engine_time_delta;
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
				if (glfwGetKey(engine_window, GLFW_KEY_X)) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				} else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
			}
		} // END INPUT

		//projection
		glfwGetWindowSize(engine_window, &engine_window_size_x,
				&engine_window_size_y);
		float aspect = (float)engine_window_size_x / (float)engine_window_size_y;
		engine_active_camera.projection = matrix4_perspective(deg2rad(90), aspect, 0.1f, 1000.0f);

		{ // draw
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDisable(GL_DEPTH_TEST);
			engine_active_camera.transform.matrix = matrix4_identity();
			skybox.transform.rotation = quaternion_conjugate(engine_active_camera.transform.rotation);
			//cube_draw(&skybox);
			glEnable(GL_DEPTH_TEST);

			transform_calculate_view_matrix(&engine_active_camera.transform);

			light.diffuse.x = 1 - sinf(engine_time_current);
			light.diffuse.y = sinf(engine_time_current);
			light.diffuse.z = 1 - sinf(engine_time_current);

			sphere.transform.rotation = quaternion_from_euler(vector3_up(engine_time_current*0.1));
			sphere_draw(&sphere);
			//for (size_t i = 0; i < engine_time_current_frame; i++) {
			//	if (i >= cubes.length) break;
			//	cube_draw(&cubes.data[i]);
			//}

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

	glfwTerminate();
	return 0;
}
