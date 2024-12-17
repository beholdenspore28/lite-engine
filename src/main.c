#include "gl.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define BLIB_IMPLEMENTATION
#include "blib/blib.h"
#include "blib/blib_json.h"
#include "blib/blib_math.h"

#include "lite_engine.h"
#include "ecs.h"
#include "oct_tree.h"

typedef struct skybox {
	mesh_t mesh;
	GLuint shader;
	material_t material;
	transform_t transform;
} skybox_t;

typedef struct kinematic_body {
	float mass;
	vec3_t acceleration;
	vec3_t velocity;
} kinematic_body_t;

int light;

static inline void oct_tree_draw(oct_tree_t *tree, vec4_t color) {
	transform_t t = (transform_t){
		.position = tree->position,
		.rotation = quat_identity(),
		.scale = vec3_one(tree->octSize),
	};
	if (tree->isSubdivided) {
		oct_tree_draw(tree->frontNorthEast, color);
		oct_tree_draw(tree->frontNorthWest, color);
		oct_tree_draw(tree->frontSouthEast, color);
		oct_tree_draw(tree->frontSouthWest, color);
		oct_tree_draw(tree->backNorthEast, color);
		oct_tree_draw(tree->backNorthWest, color);
		oct_tree_draw(tree->backSouthEast, color);
		oct_tree_draw(tree->backSouthWest, color);
	}else {
		primitive_draw_cube(t, true, color);
	}
}

static inline void kinematic_body_update(
		kinematic_body_t* kbodies, 
		transform_t* transforms) {
	oct_tree_t *tree = oct_tree_alloc();
	tree->octSize = 10000;
	tree->minimumSize = 10;
 
	for(int e = 1; e < ENTITY_COUNT_MAX; e++) {
		if (!ecs_component_exists(e, COMPONENT_KINEMATIC_BODY)) {
			continue;
		}

		assert(ecs_component_exists(e, COMPONENT_TRANSFORM));
		assert(kbodies[e].mass > 0);
		
		{ // apply forces
			//kbodies[e].acceleration = vec3_scale(
					//vec3_left(0.1), 1/kbodies[e].mass);
			kbodies[e].velocity = vec3_add(
					kbodies[e].velocity, kbodies[e].acceleration);
	
			transforms[e].position = vec3_kinematic_equation(
				kbodies[e].acceleration,
				kbodies[e].velocity,
				transforms[e].position,
				lite_engine_get_context().time_delta);
		}
		
		{ // oct tree insertion
			oct_tree_entry_t entry = (oct_tree_entry_t) {
				.position = transforms[e].position,
				.ID = e, };
			oct_tree_insert(tree, entry);
			if (!oct_tree_contains(tree, transforms[e].position) && 
				ecs_component_exists(e, COMPONENT_MESH))
					ecs_component_remove(e, COMPONENT_MESH);			
		}
	}

	const vec4_t primitive_color = { 0.2, 0.2, 0.2, 1.0 };
	oct_tree_draw(tree, primitive_color);
	oct_tree_free(tree);
}

static inline void mesh_update(
		mesh_t* meshes, 
		transform_t* transforms, 
		GLuint* shaders,
		material_t* material,
		pointLight_t* point_lights) {
	glEnable(GL_CULL_FACE);

	int window_size_x = lite_engine_get_context().window_size_x;
	int window_size_y = lite_engine_get_context().window_size_y;
	// projection
	glfwGetWindowSize(
			lite_engine_get_context().window, 
			&window_size_x,
			&window_size_y);
	float aspect = (float)lite_engine_get_context().window_size_x /
		(float)lite_engine_get_context().window_size_y;
	lite_engine_get_context().active_camera->projection =
		mat4_perspective(deg2rad(60), aspect, 0.0001f, 1000.0f);
	lite_engine_get_context().active_camera->transform.matrix = 
		mat4_identity();
	transform_calculate_view_matrix(
			&lite_engine_get_context().active_camera->transform);

	for(int e = 1; e < ENTITY_COUNT_MAX; e++) {
		if (meshes[e].use_wire_frame) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		{// ensure the entity has the required components.
			if (!ecs_component_exists(e, COMPONENT_MESH))
				continue;
			assert(ecs_component_exists(e, COMPONENT_TRANSFORM));
			assert(ecs_component_exists(e, COMPONENT_SHADER));
			assert(ecs_component_exists(e, COMPONENT_MATERIAL)); 
		}

		{ // draw
			glUseProgram(shaders[e]);

			// model matrix uniform
			transform_calculate_matrix(&transforms[e]);

			shader_setUniformM4(shaders[e], "u_modelMatrix", 
					&transforms[e].matrix);

			// view matrix uniform
			shader_setUniformM4(shaders[e], "u_viewMatrix",
				&lite_engine_get_context().active_camera->transform.matrix);

			// projection matrix uniform
			shader_setUniformM4(shaders[e], "u_projectionMatrix",
				&lite_engine_get_context().active_camera->projection);

			// camera position uniform
			shader_setUniformV3(shaders[e], "u_cameraPos",
				lite_engine_get_context().active_camera->transform.position);

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
					lite_engine_get_context().ambient_light);

			// draw
			glBindVertexArray(meshes[e].VAO);
			glDrawElements( GL_TRIANGLES, meshes[e].indices_length, GL_UNSIGNED_INT, 0);
		}
	}
	glUseProgram(0);
}

static inline void skybox_update(skybox_t* skybox) {
	// setup
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_FRONT);

	// skybox should always appear static and never move
	lite_engine_get_context().active_camera->transform.matrix = 
		mat4_identity();
	skybox->transform.rotation = quat_conjugate(
			lite_engine_get_context().active_camera->transform.rotation);

	{ // draw
		glUseProgram(skybox->shader);

		// model matrix
		transform_calculate_matrix(&skybox->transform);
		shader_setUniformM4(skybox->shader, "u_modelMatrix", 
			&skybox->transform.matrix);

		// view matrix
		shader_setUniformM4(skybox->shader, "u_viewMatrix",
			&lite_engine_get_context().active_camera->transform.matrix);

		// projection matrix
		shader_setUniformM4(skybox->shader, "u_projectionMatrix",
			&lite_engine_get_context().active_camera->projection);

		// camera position
		shader_setUniformV3(skybox->shader, "u_cameraPos",
			lite_engine_get_context().active_camera->transform.position);

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
			lite_engine_get_context().ambient_light);

		// draw
		glBindVertexArray(skybox->mesh.VAO);
		glDrawElements( GL_TRIANGLES, skybox->mesh.indices_length, GL_UNSIGNED_INT, 0);
	}

	// cleanup
	glCullFace(GL_BACK);
}

static inline void input_update(vec3_t* mouseLookVector) {   // INPUT
	{ // mouse look
		static bool firstMouse = true;
		double mouseX, mouseY;
		glfwGetCursorPos(lite_engine_get_context().window, 
				&mouseX, &mouseY);

		if (firstMouse) {
			lite_engine_get_context().active_camera->lastX = mouseX;
			lite_engine_get_context().active_camera->lastY = mouseY;
			firstMouse = false;
		}

		float xoffset = 
			mouseX - lite_engine_get_context().active_camera->lastX;
		float yoffset = 
			mouseY - lite_engine_get_context().active_camera->lastY;

		lite_engine_get_context().active_camera->lastX = mouseX;
		lite_engine_get_context().active_camera->lastY = mouseY;

		mouseLookVector->x += yoffset * 
			lite_engine_get_context().active_camera->lookSensitivity;
		mouseLookVector->y += xoffset * 
			lite_engine_get_context().active_camera->lookSensitivity;

		mouseLookVector->y = loop(mouseLookVector->y, 2 * PI);
		mouseLookVector->x = clamp(mouseLookVector->x, -PI * 0.5, PI * 0.5);

		vec3_scale(*mouseLookVector, 
				lite_engine_get_context().time_delta);

		lite_engine_get_context().active_camera->transform.rotation =
			quat_from_euler(*mouseLookVector);
	}

	{ // movement
		float cameraSpeed = 32 * lite_engine_get_context().time_delta;
		float cameraSpeedCurrent;
		if (glfwGetKey( lite_engine_get_context().window, 
					GLFW_KEY_LEFT_CONTROL)) {
			cameraSpeedCurrent = 4 * cameraSpeed;
		} else {
			cameraSpeedCurrent = cameraSpeed;
		}
		vec3_t movement = vec3_zero();

		movement.x = glfwGetKey(lite_engine_get_context().window, GLFW_KEY_D) -
			glfwGetKey(lite_engine_get_context().window, GLFW_KEY_A);
		movement.y = glfwGetKey(lite_engine_get_context().window, GLFW_KEY_SPACE) -
			glfwGetKey(lite_engine_get_context().window, GLFW_KEY_LEFT_SHIFT);
		movement.z = glfwGetKey(lite_engine_get_context().window, GLFW_KEY_W) -
			glfwGetKey(lite_engine_get_context().window, GLFW_KEY_S);

		movement = vec3_normalize(movement);
		movement = vec3_scale(movement, cameraSpeedCurrent);
		movement =
			vec3_rotate(movement, lite_engine_get_context().active_camera->transform.rotation);

		lite_engine_get_context().active_camera->transform.position =
			vec3_add(lite_engine_get_context().active_camera->transform.position, movement);

		if (glfwGetKey(lite_engine_get_context().window, GLFW_KEY_BACKSPACE)) {
			lite_engine_get_context().active_camera->transform.position = vec3_zero();
			lite_engine_get_context().active_camera->transform.rotation = quat_identity();
		}
	}
}

mesh_t mesh_lmod_alloc(const char* file_path) {
	file_buffer fb = file_buffer_alloc(file_path);
	if (fb.error) {
		fprintf(stderr, "\nfailed to open .lmod file at '%s' did you specity the correct path?\n", file_path);
		assert(0);
	}

	list_vec3_t normals   = list_vec3_t_alloc();
	list_vec3_t positions = list_vec3_t_alloc();
	list_GLuint indices   = list_GLuint_alloc();

	enum {
		STATE_INITIAL = -1,
		STATE_POSITION,
		STATE_INDICES,
		STATE_NORMAL,
	};
	uint8_t state = STATE_INITIAL;

	for(char *c = fb.text; c < fb.text+fb.length; c++) {

		if (isspace(*c)) {
			continue;
		} else if (*c == '#') {
			while(*c != '\n' && *c != '\0') { c++; }
			continue;
		}

		char token[128];
		{
			int num_tokens = sscanf(c, "%s", token);
			if (num_tokens != 1) {
				fprintf(stderr, "\n[LMOD_PARSE] failed to read token at %s\n", file_path);
				assert(0);
			}
		}

		if (strcmp(token, "vertex_indices:") == 0 || state == STATE_INDICES) {
			if (state != STATE_INDICES)
				c += sizeof("vertex_indices:");

			state = STATE_INDICES;

			GLuint index;
			int num_tokens = sscanf(c, "%u", &index);
			if (num_tokens != 1) {
				fprintf(stderr, "\n[LMOD_PARSE] failed to read vertex_indices at %s\n", file_path);
				assert(0);
			}
			while(*c != ' ' && *c != '\0') { c++; } // skip the rest of the number

			//printf("%u ", index);
			list_GLuint_add(&indices, index);
		}

		if (strcmp(token, "vertex_normals:") == 0 || state == STATE_NORMAL) {
			if (state != STATE_NORMAL)
				c += sizeof("vertex_normals:");

			state = STATE_NORMAL;

			vec3_t normal = {0};
			int num_tokens = sscanf(c, "%f %f %f", &normal.x, &normal.y, &normal.z);
			if (num_tokens != 3) {
				fprintf(stderr, "\n[LMOD_PARSE] failed to read vertex_normals at %s\n", file_path);
				assert(0);
			}

			//vec3_print(normal, "normal");
			list_vec3_t_add(&normals, normal);
			while (*c != '\n' && *c != '\0') { c++; } 
		}

		if (strcmp(token, "vertex_positions:") == 0 || state == STATE_POSITION) {
			if (state != STATE_POSITION)
				c += sizeof("vertex_positions:");

			state = STATE_POSITION;

			vec3_t position = {0};
			int num_tokens = sscanf(c, "%f %f %f\n", &position.x, &position.y, &position.z);
			if (num_tokens != 3) {
				fprintf(stderr, "\n[LMOD_PARSE] failed to read vertex_positions at %s\n", file_path);
				assert(0);
			}

			//vec3_print(position, "position");
			list_vec3_t_add(&positions, position);
			while (*c != '\n' && *c != '\0') { c++; } 
		}
	}

	list_vertex_t vertices = list_vertex_t_alloc();
	for(size_t i = 0; i < positions.length; i++) {
		vertex_t vertex = {0};
		vertex.position = positions.array[i];
		list_vertex_t_add(&vertices, vertex);
	}

	mesh_t mesh = mesh_alloc(vertices.array, vertices.length, 
			                 indices.array,  indices.length);
	return mesh;
}

int main() {
	printf("Rev up those fryers!\n");

#if 0
	json_value *json = json_read("res/test.json");
	json_print(json);
	json_free(json);
#endif

	// init engine
	lite_engine_context_t* context = malloc(sizeof(lite_engine_context_t));
	context->window_size_x        = 1920/2;
	context->window_size_y        = 1080/2;
	context->window_position_x    = 0;
	context->window_position_y    = 0;
	context->window_title         = "Game Window";
	context->window_fullscreen    = false;
	context->window_always_on_top = false;
	context->time_current         = 0.0f;
	context->time_last            = 0.0f;
	context->time_delta           = 0.0f;
	context->time_FPS             = 0.0f;
	context->frame_current        = 0;
	context->ambient_light        = (vec3_t) {0.1, 0.1, 0.1};

	// yes this looks silly but it helps to easily support
	// multiple cameras
	camera_t* camera              = malloc(sizeof(camera_t));
	camera->transform.position    = (vec3_t){0.0, 0.0, -25.0};
	camera->transform.rotation    = quat_identity();
	camera->transform.scale       = vec3_one(1.0);
	camera->projection            = mat4_identity();
	camera->lookSensitivity       = 0.002f;

	context->active_camera        = camera;

	lite_engine_set_context(context);
	lite_engine_start();

	lite_engine_set_clear_color(0.0, 0.0, 0.0, 1.0);

	GLuint unlitShader = shader_create(
			"res/shaders/unlit.vs.glsl", 
			"res/shaders/unlit.fs.glsl");

	GLuint diffuseShader = shader_create(
			"res/shaders/diffuse.vs.glsl",
			"res/shaders/diffuse.fs.glsl");

	GLuint testDiffuseMap = texture_create("res/textures/test.png");
	mesh_t testLmod = mesh_lmod_alloc("res/models/untitled.lmod");

	// allocate component data
	mesh_t* mesh = calloc(sizeof(mesh_t),ENTITY_COUNT_MAX);
	GLuint* shader = calloc(sizeof(GLuint),ENTITY_COUNT_MAX);
	material_t* material = calloc(sizeof(material_t),ENTITY_COUNT_MAX);
	transform_t* transforms = calloc(sizeof(transform_t),ENTITY_COUNT_MAX);
	kinematic_body_t* kinematic_body = calloc(sizeof(kinematic_body_t),ENTITY_COUNT_MAX);
	pointLight_t* point_light = calloc(sizeof(pointLight_t), ENTITY_COUNT_MAX);

	ecs_alloc(); 

	int suzanne = ecs_entity_create();
	{
		ecs_component_add(suzanne, COMPONENT_TRANSFORM);
		ecs_component_add(suzanne, COMPONENT_MESH);
		ecs_component_add(suzanne, COMPONENT_MATERIAL);
		ecs_component_add(suzanne, COMPONENT_SHADER);

		mesh[suzanne] = testLmod;
		mesh[suzanne].use_wire_frame = true;
		shader[suzanne] = unlitShader;
		material[suzanne] = (material_t){
			.diffuseMap = testDiffuseMap,
		};
		transforms[suzanne] = (transform_t){
			.position = vec3_zero(),
			.rotation = quat_identity(),
			.scale = vec3_one(10.0), 
		};
	}

#if 0 // lmod vertex position preview
	for(int i = 0; i < testLmod.vertices_length; i++) {
		int cube = ecs_entity_create();

		ecs_component_add(cube, COMPONENT_TRANSFORM);
		ecs_component_add(cube, COMPONENT_MESH);
		ecs_component_add(cube, COMPONENT_MATERIAL);
		ecs_component_add(cube, COMPONENT_SHADER);

		mesh[cube] = mesh_alloc_cube();
		shader[cube] = unlitShader;
		material[cube] = (material_t){
			.diffuseMap = testDiffuseMap, };
		transforms[cube] = (transform_t){
			.position = vec3_scale(testLmod.vertices[i].position, transforms[suzanne].scale.x),					
			.rotation = quat_identity(),
			.scale = vec3_one(0.25), };
	}
#endif

#if 0
	// create cubes
	for (int i = 1; i <= 1000; i++) {
		int cube = ecs_entity_create();

		ecs_component_add(cube, COMPONENT_KINEMATIC_BODY);
		ecs_component_add(cube, COMPONENT_TRANSFORM);
		ecs_component_add(cube, COMPONENT_MESH);
		ecs_component_add(cube, COMPONENT_MATERIAL);
		ecs_component_add(cube, COMPONENT_SHADER);

		mesh[cube] = mesh_alloc_cube();
		shader[cube] = diffuseShader;
		material[cube] = (material_t){
			.diffuseMap = testDiffuseMap, };
		transforms[cube] = (transform_t){
			.position = (vec3_t){
				(float)noise1(i    ) * 1000 - 500,
				(float)noise1(i + 1) * 1000 - 500,
				(float)noise1(i + 2) * 1000 - 500},
			.rotation = quat_identity(),
			.scale = vec3_one(5.0), };
		kinematic_body[cube].velocity = vec3_zero();
		kinematic_body[cube].mass = 1.0;
	}
#endif

	// create skybox
	skybox_t skybox = (skybox_t) {
		.mesh =   mesh_alloc_cube(),
		.shader = unlitShader,
		.material = (material_t){
			.diffuseMap = texture_create("res/textures/space.png"),
		},
		.transform = (transform_t){
			.position = { 0.0, 0.0, 0.0 },
			.rotation = quat_identity(),
			.scale = vec3_one(100000.0),
		},
	};

	// create light
	light = ecs_entity_create();
	ecs_component_add(light, COMPONENT_POINT_LIGHT);
	ecs_component_add(light, COMPONENT_TRANSFORM);
	point_light[light] = (pointLight_t){
		.diffuse = vec3_one(0.8f),
		.specular = vec3_one(1.0f),
		.constant = 1.0f,
		.linear = 0.09f,
		.quadratic = 0.032f,
	};
	transforms[light].position = (vec3_t){20, 20, 20};


	vec3_t mouseLookVector = vec3_zero();

	while (lite_engine_is_running()) {
		lite_engine_update();
		input_update(&mouseLookVector);
		transforms[suzanne].rotation = quat_multiply(transforms[suzanne].rotation,
				quat_from_euler( vec3_up(lite_engine_get_context().time_delta)));
		mesh_update(mesh, transforms, shader, material, point_light);
		kinematic_body_update(kinematic_body, transforms);
		//skybox_update(&skybox);
	}

	free(mesh);
	free(shader);
	free(material);
	free(transforms);
	free(kinematic_body);
	free(point_light);
	free(camera);

	lite_engine_stop();
	return 0;
}
