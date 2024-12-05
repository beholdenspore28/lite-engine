#include "gl.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define B_LIST_IMPLEMENTATION
#include "blib/blib.h"
#include "blib/blib_math.h"
#include "oct_tree.h"

DEFINE_LIST(vector3_t)
DEFINE_LIST(matrix4_t)
DEFINE_LIST(quaternion_t)

// TODO move all of this global state to an engine object
typedef struct lite_engine_context {
	GLFWwindow *window;
	int         window_size_x;
	int         window_size_y;
	int         window_position_x;
	int         window_position_y;
	char       *window_title;
	bool        window_fullscreen;
	bool        window_always_on_top;
	float       time_current;
	float       time_last;
	float       time_delta;
	float       time_FPS;
	uint64_t    frame_current;
	vector3_t   ambient_light;
	camera_t    active_camera;
} lite_engine_context_t;

static lite_engine_context_t* lite_engine_context_current = NULL;

typedef struct skybox {
	mesh_t mesh;
	GLuint shader;
	material_t material;
	transform_t transform;
} skybox_t;

typedef struct pointLight {
	float constant;
	float linear;
	float quadratic;
	vector3_t diffuse;
	vector3_t specular;
} pointLight_t;

typedef struct kinematic_body {
	float mass;
	vector3_t position;
	vector3_t acceleration;
	vector3_t velocity;
} kinematic_body_t;

int light;

GLuint gizmo_shader;
mesh_t gizmo_mesh_cube;

void gizmo_draw_cube(transform_t transform, bool wireframe, vector4_t color) {
	glDisable(GL_CULL_FACE);
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(gizmo_shader);

	// model matrix
	transform_calculate_matrix(&transform);

	shader_setUniformM4(gizmo_shader, "u_modelMatrix", &transform.matrix);

	// view matrix
	shader_setUniformM4(gizmo_shader, "u_viewMatrix",
			&lite_engine_context_current->active_camera.transform.matrix);

	// projection matrix
	shader_setUniformM4(gizmo_shader, "u_projectionMatrix",
			&lite_engine_context_current->active_camera.projection);

	shader_setUniformV4(gizmo_shader, "u_color", color);

	glBindVertexArray(gizmo_mesh_cube.VAO);
	glDrawElements( GL_TRIANGLES, gizmo_mesh_cube.
			indexCount, GL_UNSIGNED_INT, 0);
}

void gizmo_draw_oct_tree(oct_tree_t *tree, vector4_t color) {
	transform_t t = (transform_t){
		.position = tree->position,
		.rotation = quaternion_identity(),
		.scale = vector3_one(tree->octSize),
	};
	if (tree->isSubdivided) {
		gizmo_draw_oct_tree(tree->frontNorthEast, color);
		gizmo_draw_oct_tree(tree->frontNorthWest, color);
		gizmo_draw_oct_tree(tree->frontSouthEast, color);
		gizmo_draw_oct_tree(tree->frontSouthWest, color);
		gizmo_draw_oct_tree(tree->backNorthEast, color);
		gizmo_draw_oct_tree(tree->backNorthWest, color);
		gizmo_draw_oct_tree(tree->backSouthEast, color);
		gizmo_draw_oct_tree(tree->backSouthWest, color);
	}else {
		gizmo_draw_cube(t, true, color);
	}
}

// set window resolution
void lite_engine_window_set_resolution(const int x, const int y) {
	glfwSetWindowSize(lite_engine_context_current->window, x, y);
}

// position window in the center of the screen
void lite_engine_window_set_position(const int x, const int y) {
	glfwSetWindowPos(lite_engine_context_current->window, x, y);
}

void lite_engine_start(void) {
	assert(lite_engine_context_current != NULL);
	if (!glfwInit()) {
		printf("[ERROR_GLFW] Failed to initialize GLFW");
	}

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	if (lite_engine_context_current->window_always_on_top) {
		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	}
	assert(lite_engine_context_current->window_title != NULL);

	if (lite_engine_context_current->window_fullscreen) {
		lite_engine_context_current->window =
			glfwCreateWindow(
					lite_engine_context_current->window_size_x, 
					lite_engine_context_current->window_size_y,
					lite_engine_context_current->window_title, 
					glfwGetPrimaryMonitor(), NULL);
	} else {
		lite_engine_context_current->window = glfwCreateWindow(
				lite_engine_context_current->window_size_x, 
				lite_engine_context_current->window_size_y,
				lite_engine_context_current->window_title, NULL, NULL);
	}
	assert(lite_engine_context_current->window != NULL);

	glfwSetWindowPos(lite_engine_context_current->window, 
			lite_engine_context_current->window_position_x,
			lite_engine_context_current->window_position_y);
	glfwShowWindow(lite_engine_context_current->window);
	glfwMakeContextCurrent(lite_engine_context_current->window);
	glfwSetKeyCallback(lite_engine_context_current->window, key_callback);
	glfwSetFramebufferSizeCallback(
			lite_engine_context_current->window, 
			framebuffer_size_callback);
	glfwSetInputMode(lite_engine_context_current->window, 
			GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSwapInterval(0);

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gizmo_shader = shader_create(
		"res/shaders/gizmos.vs.glsl",
		"res/shaders/gizmos.fs.glsl");
	gizmo_mesh_cube = mesh_alloc_cube();
}

void engine_set_clear_color(const float r, const float g, const float b,
		const float a) {
	glClearColor((GLfloat)r, (GLfloat)g, (GLfloat)b, (GLfloat)a);
}

static int* entities;
static int entity_count = 0;
enum ENTITY_ENUM { ENTITY_NULL, ENTITY_COUNT_MAX = 6000, };

static int** components;

enum COMPONENT_TYPE { 
	COMPONENT_NULL,
	COMPONENT_KINEMATIC_BODY, 
	COMPONENT_COLLIDER, 
	COMPONENT_TRANSFORM,
	COMPONENT_POINT_LIGHT,
	COMPONENT_MESH,
	COMPONENT_MATERIAL,
	COMPONENT_SHADER,
	COMPONENT_COUNT_MAX,
};

void ecs_alloc(void) {
	entities = calloc(sizeof(entities), ENTITY_COUNT_MAX);
	components = calloc(sizeof(int**), ENTITY_COUNT_MAX);
	for(int i = 0; i < ENTITY_COUNT_MAX; i++) {
		components[i] = calloc(sizeof(int*), COMPONENT_COUNT_MAX);
	}
}

int ecs_entity_create(void) {
	++entity_count;
	assert(entity_count != ENTITY_NULL);
	assert(entity_count <= ENTITY_COUNT_MAX);
	return entity_count;
}

int ecs_component_add(int entity, int component) {
	components[entity][component] = 1;
	return component;
}

static inline vector3_t kinematic_equation(
		vector3_t acceleration, 
		vector3_t velocity,
		vector3_t position, 
		float time) {
	float x = 0.5f * acceleration.x * time * time + velocity.x * time + position.x;
	float y = 0.5f * acceleration.y * time * time + velocity.y * time + position.y;
	float z = 0.5f * acceleration.z * time * time + velocity.z * time + position.z;
	return (vector3_t){x, y, z};
}

void kinematic_body_update(
		kinematic_body_t* kbodies, 
		transform_t* transforms) {
	oct_tree_t *tree = oct_tree_alloc();
	tree->octSize = 10000;
	tree->minimumSize = 10;
 
	for(int e = 1; e < ENTITY_COUNT_MAX; e++) {
		if (!components[e][COMPONENT_KINEMATIC_BODY])
			continue;
		assert(components[e][COMPONENT_TRANSFORM]);
		assert(kbodies[e].mass > 0);
		
		{ // apply forces
			kbodies[e].acceleration = vector3_scale(vector3_left(0.01), 1/kbodies[e].mass);
			kbodies[e].velocity = vector3_add(kbodies[e].velocity, kbodies[e].acceleration);
	
			kbodies[e].position = kinematic_equation(
				kbodies[e].acceleration,
				kbodies[e].velocity,
				kbodies[e].position,
				lite_engine_context_current->time_delta);
			transforms[e].position = kbodies[e].position;
		}
		
		{ // oct tree insertion
			oct_tree_entry_t entry = (oct_tree_entry_t) {
				.position = kbodies[e].position,
				.ID = e, };
			oct_tree_insert(tree, entry);
			if (!oct_tree_contains(tree, kbodies[e].position))
				components[e][COMPONENT_MESH] = 0;			
		}

	}

	const vector4_t gizmo_color = { 0.2, 0.2, 0.2, 1.0 };
	gizmo_draw_oct_tree(tree, gizmo_color);
	oct_tree_free(tree);
}

void mesh_update(
		mesh_t* meshes, 
		transform_t* transforms, 
		GLuint* shaders,
		material_t* material,
		pointLight_t* point_lights) {
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// projection
	glfwGetWindowSize(
			lite_engine_context_current->window, 
			&lite_engine_context_current->window_size_x,
			&lite_engine_context_current->window_size_y);
	float aspect = (float)lite_engine_context_current->window_size_x /
		(float)lite_engine_context_current->window_size_y;
	lite_engine_context_current->active_camera.projection =
		matrix4_perspective(deg2rad(60), aspect, 0.0001f, 1000.0f);
	lite_engine_context_current->active_camera.transform.matrix = 
		matrix4_identity();
	transform_calculate_view_matrix(
			&lite_engine_context_current->active_camera.transform);

	for(int e = 1; e < ENTITY_COUNT_MAX; e++) {
		{// ensure the entity has the required components.
			if (!components[e][COMPONENT_MESH])
				continue;
			assert(components[e][COMPONENT_TRANSFORM]);
			assert(components[e][COMPONENT_SHADER]);
			assert(components[e][COMPONENT_MATERIAL]); 
		}

		{ // draw
			glUseProgram(shaders[e]);

			// model matrix uniform
			transform_calculate_matrix(&transforms[e]);

			shader_setUniformM4(shaders[e], "u_modelMatrix", 
					&transforms[e].matrix);

			// view matrix uniform
			shader_setUniformM4(shaders[e], "u_viewMatrix",
				&lite_engine_context_current->active_camera.transform.matrix);

			// projection matrix uniform
			shader_setUniformM4(shaders[e], "u_projectionMatrix",
				&lite_engine_context_current->active_camera.projection);

			// camera position uniform
			shader_setUniformV3(shaders[e], "u_cameraPos",
				lite_engine_context_current->active_camera.transform.position);

			// light uniforms
			shader_setUniformV3(shaders[e], "u_light.position",
					transforms[light].position);
			shader_setUniformFloat(shaders[e], "u_light.constant",
					point_lights[light].constant);
			shader_setUniformFloat(shaders[e], "u_light.linear",
					point_lights[light].linear);
			shader_setUniformFloat(shaders[e], "u_light.quadratic",
					point_lights[light].quadratic);
			shader_setUniformV3(shaders[e], "u_light.diffuse",
					point_lights[light].diffuse);
			shader_setUniformV3(shaders[e], "u_light.specular",
					point_lights[light].specular);

			// textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material[e].diffuseMap);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, material[e].specularMap);

			// other material properties
			shader_setUniformInt(shaders[e], "u_material.diffuse", 0);
			shader_setUniformInt(shaders[e], "u_material.specular", 1);
			shader_setUniformFloat(shaders[e], "u_material.shininess", 32.0f);
			shader_setUniformV3(shaders[e], "u_ambientLight",
					lite_engine_context_current->ambient_light);

			// draw
			glBindVertexArray(meshes[e].VAO);
			glDrawElements(
				GL_TRIANGLES, 
				meshes[e].indexCount, 
				GL_UNSIGNED_INT, 0);
		}
	}
	glUseProgram(0);
}

void skybox_update(skybox_t* skybox) {
	// setup
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_FRONT);

	// skybox should always appear static and never move
	lite_engine_context_current->active_camera.transform.matrix = 
		matrix4_identity();
	skybox->transform.rotation = quaternion_conjugate(
			lite_engine_context_current->active_camera.transform.rotation);

	{ // draw
		glUseProgram(skybox->shader);

		// model matrix
		transform_calculate_matrix(&skybox->transform);
		shader_setUniformM4(skybox->shader, "u_modelMatrix", 
			&skybox->transform.matrix);

		// view matrix
		shader_setUniformM4(skybox->shader, "u_viewMatrix",
			&lite_engine_context_current->active_camera.transform.matrix);

		// projection matrix
		shader_setUniformM4(skybox->shader, "u_projectionMatrix",
			&lite_engine_context_current->active_camera.projection);

		// camera position
		shader_setUniformV3(skybox->shader, "u_cameraPos",
			lite_engine_context_current->active_camera.transform.position);

		// textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skybox->material.diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, skybox->material.specularMap);

		// other material properties
		shader_setUniformInt(skybox->shader, "u_material.diffuse", 0);
		shader_setUniformInt(skybox->shader, "u_material.specular", 1);
		shader_setUniformFloat(skybox->shader, "u_material.shininess", 32.0f);
		shader_setUniformV3(skybox->shader, "u_ambientLight",
			lite_engine_context_current->ambient_light);

		// draw
		glBindVertexArray(skybox->mesh.VAO);
		glDrawElements(
				GL_TRIANGLES, 
				skybox->mesh.indexCount,
				GL_UNSIGNED_INT, 0);
	}

	// cleanup
	glCullFace(GL_BACK);
}

void input_update(vector3_t* mouseLookVector) {   // INPUT
	{ // mouse look
		static bool firstMouse = true;
		double mouseX, mouseY;
		glfwGetCursorPos(lite_engine_context_current->window, 
				&mouseX, &mouseY);

		if (firstMouse) {
			lite_engine_context_current->active_camera.lastX = mouseX;
			lite_engine_context_current->active_camera.lastY = mouseY;
			firstMouse = false;
		}

		float xoffset = 
			mouseX - lite_engine_context_current->active_camera.lastX;
		float yoffset = 
			mouseY - lite_engine_context_current->active_camera.lastY;

		lite_engine_context_current->active_camera.lastX = mouseX;
		lite_engine_context_current->active_camera.lastY = mouseY;

		mouseLookVector->x += yoffset * 
			lite_engine_context_current->active_camera.lookSensitivity;
		mouseLookVector->y += xoffset * 
			lite_engine_context_current->active_camera.lookSensitivity;

		mouseLookVector->y = loop(mouseLookVector->y, 2 * PI);
		mouseLookVector->x = clamp(mouseLookVector->x, -PI * 0.5, PI * 0.5);

		vector3_scale(*mouseLookVector, 
				lite_engine_context_current->time_delta);

		lite_engine_context_current->active_camera.transform.rotation =
			quaternion_from_euler(*mouseLookVector);
	}

	{ // movement
		float cameraSpeed = 32 * lite_engine_context_current->time_delta;
		float cameraSpeedCurrent;
		if (glfwGetKey( lite_engine_context_current->window, 
					GLFW_KEY_LEFT_CONTROL)) {
			cameraSpeedCurrent = 4 * cameraSpeed;
		} else {
			cameraSpeedCurrent = cameraSpeed;
		}
		vector3_t movement = vector3_zero();

		movement.x = glfwGetKey(lite_engine_context_current->window, GLFW_KEY_D) -
			glfwGetKey(lite_engine_context_current->window, GLFW_KEY_A);
		movement.y = glfwGetKey(lite_engine_context_current->window, GLFW_KEY_SPACE) -
			glfwGetKey(lite_engine_context_current->window, GLFW_KEY_LEFT_SHIFT);
		movement.z = glfwGetKey(lite_engine_context_current->window, GLFW_KEY_W) -
			glfwGetKey(lite_engine_context_current->window, GLFW_KEY_S);

		movement = vector3_normalize(movement);
		movement = vector3_scale(movement, cameraSpeedCurrent);
		movement =
			vector3_rotate(movement, lite_engine_context_current->active_camera.transform.rotation);

		lite_engine_context_current->active_camera.transform.position =
			vector3_add(lite_engine_context_current->active_camera.transform.position, movement);

		if (glfwGetKey(lite_engine_context_current->window, GLFW_KEY_BACKSPACE)) {
			lite_engine_context_current->active_camera.transform.position = vector3_zero();
			lite_engine_context_current->active_camera.transform.rotation = quaternion_identity();
		}
	}
}

void engine_time_update(void) { // update time
	lite_engine_context_current->time_current = glfwGetTime();
	lite_engine_context_current->time_delta = lite_engine_context_current->time_current - lite_engine_context_current->time_last;
	lite_engine_context_current->time_last = lite_engine_context_current->time_current;

	lite_engine_context_current->time_FPS = 1 / lite_engine_context_current->time_delta;
	lite_engine_context_current->frame_current++;

#if 0 // log time
	printf("time_current   %f\n"
			"time_last     %f\n"
			"time_delta    %f\n"
			"FPS           %f\n"
			"frame_current %lu\n",
			lite_engine_context_current->time_current, 
			lite_engine_context_current->time_last, 
			lite_engine_context_current->time_delta,
			lite_engine_context_current->time_FPS, 
			lite_engine_context_current->frame_current);
#endif // log time
}

int main(void) {
	printf("Rev up those fryers!\n");

	// init engine
	lite_engine_context_t* context = malloc(sizeof(lite_engine_context_t));
	context->window_size_x        = 1920/2;
	context->window_size_y        = 1080/2;
	context->window_position_x    = 1920/2;
	context->window_position_y    = 0;
	context->window_title         = "Game Window";
	context->window_fullscreen    = false;
	context->window_always_on_top = false;
	context->time_current         = 0.0f;
	context->time_last            = 0.0f;
	context->time_delta           = 0.0f;
	context->time_FPS             = 0.0f;
	context->frame_current        = 0;
	context->ambient_light        = (vector3_t) {0.1, 0.1, 0.1};
	context->active_camera        = (camera_t){
		.transform.position       = (vector3_t){0.0, 0.0, -600.0},
		.transform.rotation       = quaternion_identity(),
		.transform.scale          = vector3_one(1.0),
		.projection               = matrix4_identity(),
		.lookSensitivity          = 0.002f,
	};

	lite_engine_context_current = context;
	lite_engine_start();

	engine_set_clear_color(0.0, 0.0, 0.0, 1.0);

	GLuint unlitShader = shader_create( "res/shaders/unlit.vs.glsl", "res/shaders/unlit.fs.glsl");
	GLuint testDiffuseMap = texture_create("res/textures/test.png");

	// allocate component data
	mesh_t* mesh = calloc(sizeof(mesh_t),ENTITY_COUNT_MAX);
	GLuint* shader = calloc(sizeof(GLuint),ENTITY_COUNT_MAX);
	material_t* material = calloc(sizeof(material_t),ENTITY_COUNT_MAX);
	transform_t* transform = calloc(sizeof(transform_t),ENTITY_COUNT_MAX);
	kinematic_body_t* kinematic_body = calloc(sizeof(kinematic_body_t),ENTITY_COUNT_MAX);
	pointLight_t* point_light = calloc(sizeof(pointLight_t), ENTITY_COUNT_MAX);

	ecs_alloc(); 

	// create cubes
	for (int i = 1; i <= 1000; i++) {
		int cube = ecs_entity_create();

		ecs_component_add(cube, COMPONENT_KINEMATIC_BODY);
		ecs_component_add(cube, COMPONENT_TRANSFORM);
		ecs_component_add(cube, COMPONENT_MESH);
		ecs_component_add(cube, COMPONENT_MATERIAL);
		ecs_component_add(cube, COMPONENT_SHADER);

		mesh[cube] = mesh_alloc_cube();
		shader[cube] = unlitShader;
		material[cube] = (material_t){
			.diffuseMap = testDiffuseMap, };
		transform[cube] = (transform_t){
			.position = (vector3_t){
				(float)noise1(i    ) * 1000 - 500,
				(float)noise1(i + 1) * 1000 - 500,
				(float)noise1(i + 2) * 1000 - 500},
			.rotation = quaternion_identity(),
			.scale = vector3_one(5.0), };
		kinematic_body[cube].position =
			transform[cube].position;
		kinematic_body[cube].velocity = vector3_zero();
		kinematic_body[cube].mass = 1.0;
	}

	// create skybox
	skybox_t skybox = (skybox_t) {
		.mesh =   mesh_alloc_cube(),
		.shader = unlitShader,
		.material = (material_t){
			.diffuseMap = texture_create("res/textures/space.png"),
		},
		.transform = (transform_t){
			.position = { 0.0, 0.0, 0.0 },
			.rotation = quaternion_identity(),
			.scale = vector3_one(10000.0),
		},
	};

	// create light
	light = ecs_entity_create();
	ecs_component_add(light, COMPONENT_POINT_LIGHT);
	ecs_component_add(light, COMPONENT_TRANSFORM);
	point_light[light] = (pointLight_t){
		.diffuse = vector3_one(0.8f),
		.specular = vector3_one(1.0f),
		.constant = 1.0f,
		.linear = 0.09f,
		.quadratic = 0.032f,
	};
	transform[light].position = (vector3_t){5, 5, 5};


	vector3_t mouseLookVector = vector3_zero();

	while (!glfwWindowShouldClose(lite_engine_context_current->window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		engine_time_update();
		input_update(&mouseLookVector);
		mesh_update( mesh, transform, shader, material, point_light);
		kinematic_body_update(kinematic_body, transform);
		skybox_update(&skybox);
		glfwSwapBuffers(lite_engine_context_current->window);
		glfwPollEvents();
	}

	free(mesh);
	free(shader);
	free(material);
	free(transform);
	free(kinematic_body);
	free(point_light);

	glfwTerminate();
	return 0;
}
