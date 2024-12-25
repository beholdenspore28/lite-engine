#include "lite_engine.h"
#include "lite_engine_gl.h"

#define GLFW_INCLUDE_NONE
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "blib/blib.h"
#include "blib/blib_math3d.h"
#include "blib/blib_file.h"

#include <ctype.h>

DEFINE_LIST(GLuint)
DEFINE_LIST(mesh_t)
DEFINE_LIST(vertex_t)
DEFINE_LIST(transform_t)

void lite_engine_gl_transform_calculate_matrix(transform_t *t) {
	matrix4_t translation = matrix4_translate(t->position);
	matrix4_t rotation = quaternion_to_matrix4(t->rotation);
	matrix4_t scale = matrix4_scale(t->scale);
	t->matrix = matrix4_multiply(rotation, translation);
	t->matrix = matrix4_multiply(scale, t->matrix);
}

void lite_engine_gl_transform_calculate_view_matrix(transform_t *t) {
	matrix4_t translation = matrix4_translate(vector3_negate(t->position));
	matrix4_t rotation = quaternion_to_matrix4(quaternion_conjugate(t->rotation));
	matrix4_t scale = matrix4_scale(t->scale);
	t->matrix = matrix4_multiply(translation, rotation);
	t->matrix = matrix4_multiply(scale, t->matrix);
}

vector3_t transform_basis_forward(transform_t t, float magnitude) {
	return vector3_rotate(vector3_forward(magnitude), t.rotation);
}

vector3_t transform_basis_up(transform_t t, float magnitude) {
	return vector3_rotate(vector3_up(magnitude), t.rotation);
}

vector3_t transform_basis_right(transform_t t, float magnitude) {
	return vector3_rotate(vector3_right(magnitude), t.rotation);
}

vector3_t transform_basis_back(transform_t t, float magnitude) {
	return vector3_rotate(vector3_back(magnitude), t.rotation);
}

vector3_t transform_basis_down(transform_t t, float magnitude) {
	return vector3_rotate(vector3_down(magnitude), t.rotation);
}

vector3_t transform_basis_left(transform_t t, float magnitude) {
	return vector3_rotate(vector3_left(magnitude), t.rotation);
}

GLuint lite_engine_gl_texture_create(const char *imageFile) {
	/*create texture*/
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	/*set parameters*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/*load texture data from file*/
	int width, height, numChannels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(imageFile, &width, &height, &numChannels, 0);

	/*error check*/
	if (data) {
		if (numChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
					GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else if (numChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
					GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	} else {
		debug_error("Failed to load texture from '%s'", imageFile);
	}

	/*cleanup*/
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}
static GLuint internal_shader_compile(GLuint type, const char *source) {
	/*creation*/
	GLuint shader = 0;
	if (type == GL_VERTEX_SHADER) {
		shader = glCreateShader(GL_VERTEX_SHADER);
	} else if (type == GL_FRAGMENT_SHADER) {
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	}

	/*compilation*/
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	/*Error check*/
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char infoLog[length];
		glGetShaderInfoLog(shader, length, &length, infoLog);

		if (type == GL_VERTEX_SHADER) {
			debug_error("Failed to compile vertex shader\n%s\n", infoLog);
			exit(1);
		} else if (type == GL_FRAGMENT_SHADER) {
			debug_error("Failed to compile fragment shader\n%s\n", infoLog);
			exit(1);
		}
		glDeleteShader(shader);
	}
	return shader;
}

GLuint lite_engine_gl_shader_create(
		const char *vertex_shader_file_path,
		const char *fragment_shader_file_path) {

	debug_log("Loading shaders from '%s' and '%s'", 
			vertex_shader_file_path,
			fragment_shader_file_path);

	file_buffer vertex_source_string = file_buffer_alloc(vertex_shader_file_path);
	file_buffer fragment_source_string = file_buffer_alloc(fragment_shader_file_path);

	if (vertex_source_string.error == 1) {
		debug_error(
				"Failed to locate '%s'", 
				vertex_shader_file_path);
		exit(1);
	}
	if (fragment_source_string.error == 1) {
		debug_error(
				"Failed to locate '%s'", 
				fragment_shader_file_path);
		exit(1);
	}

	const char *vertSourceString = vertex_source_string.text;
	const char *fragSourceString = fragment_source_string.text;

	GLuint program = glCreateProgram();

	GLuint vertShader = internal_shader_compile(
			GL_VERTEX_SHADER, 
			vertSourceString);

	GLuint fragShader = internal_shader_compile(
			GL_FRAGMENT_SHADER, 
			fragSourceString);

	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);

	glLinkProgram(program);

	GLint success;
	GLint length;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
	char infoLog[length];
	if (!success) {
		glGetProgramInfoLog(program, length, &length, infoLog);
		debug_error("Failed to link shader\n %s", infoLog);
	}

	glValidateProgram(program);

	file_buffer_free(vertex_source_string);
	file_buffer_free(fragment_source_string);

	return program;
}

void lite_engine_gl_shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform1i(UniformLocation, i);
}

void lite_engine_gl_shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform1f(UniformLocation, f);
}

void lite_engine_gl_shader_setUniformV3(GLuint shader, const char *uniformName, vector3_t v) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform3f(UniformLocation, v.x, v.y, v.z);
}

void lite_engine_gl_shader_setUniformV4(GLuint shader, const char *uniformName, vector4_t v) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform4f(UniformLocation, v.x, v.y, v.z, v.w);
}

void lite_engine_gl_shader_setUniformM4(GLuint shader, const char *uniformName, matrix4_t *m) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, &m->elements[0]);
}
extern camera_t *internal_gl_active_camera;

void   lite_engine_gl_mesh_update (
		mesh_t        mesh,
		material_t    material,
		transform_t   transform,
		point_light_t point_light,
		transform_t   light_transform) {
	glEnable(GL_CULL_FACE);

	if (mesh.enabled == 0) {
		return;
	}

	if (mesh.use_wire_frame) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	{ // draw
		glUseProgram(material.shader);

		// model matrix uniform
		lite_engine_gl_transform_calculate_matrix(&transform);

		lite_engine_gl_shader_setUniformM4(material.shader, "u_modelMatrix", 
				&transform.matrix);

		// view matrix uniform
		lite_engine_gl_shader_setUniformM4(material.shader, "u_viewMatrix",
				&internal_gl_active_camera->transform.matrix);

		// projection matrix uniform
		lite_engine_gl_shader_setUniformM4(material.shader, "u_projectionMatrix",
				&internal_gl_active_camera->projection);

		// camera position uniform
		lite_engine_gl_shader_setUniformV3(material.shader, "u_cameraPos",
				internal_gl_active_camera->transform.position);

		// light uniforms
		lite_engine_gl_shader_setUniformV3(
				material.shader,
				"u_light.position",
				light_transform.position);

		lite_engine_gl_shader_setUniformFloat(
				material.shader,
				"u_light.constant",
				point_light.constant);

		lite_engine_gl_shader_setUniformFloat(
				material.shader,
				"u_light.linear",
				point_light.linear);

		lite_engine_gl_shader_setUniformFloat(
				material.shader,
				"u_light.quadratic",
				point_light.quadratic);

		lite_engine_gl_shader_setUniformV3(
				material.shader,
				"u_light.diffuse",
				point_light.diffuse);

		lite_engine_gl_shader_setUniformV3(
				material.shader,
				"u_light.specular",
				point_light.specular);

		// textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.specularMap);

		// other material properties
		lite_engine_gl_shader_setUniformInt(material.shader, "u_material.diffuse", 0);
		lite_engine_gl_shader_setUniformInt(material.shader, "u_material.specular", 1);
		lite_engine_gl_shader_setUniformFloat(material.shader, "u_material.shininess", 32.0f);
		lite_engine_gl_shader_setUniformV3(material.shader, "u_ambientLight",
		vector3_one(0.1));

		// draw
		glBindVertexArray(mesh.VAO);
		glDrawElements( GL_TRIANGLES, mesh.indices.length, GL_UNSIGNED_INT, 0);
	}

	glUseProgram(0);
}

mesh_t lite_engine_gl_mesh_alloc(list_vertex_t vertices, list_GLuint indices) {
	mesh_t m   = {0};
	m.enabled  = true;
	m.vertices = vertices;
	m.indices  = indices;

	glGenVertexArrays(1, &m.VAO);
	glGenBuffers(1, &m.VBO);
	glGenBuffers(1, &m.EBO);

	glBindVertexArray(m.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * vertices.length, vertices.array,
			GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.length, indices.array,
			GL_STATIC_DRAW);

	GLuint vertStride = sizeof(vertex_t); // how many bytes per vertex?

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, position));
	glEnableVertexAttribArray(0);

	// texcoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, texCoord));
	glEnableVertexAttribArray(1);

	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, normal));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return m;
}

mesh_t lite_engine_gl_mesh_lmod_alloc(const char* file_path) {
	debug_log("Loading lmod file from '%s'", file_path);
	file_buffer fb = file_buffer_alloc(file_path);

	if (fb.error) {
		debug_error(
				"Failed to open .lmod file at '%s' did you specity the correct path?", 
				file_path);
		assert(0);
	}

	list_vector3_t positions  = list_vector3_t_alloc();
	list_vector3_t normals    = list_vector3_t_alloc();
	list_vector2_t tex_coords = list_vector2_t_alloc();
	list_GLuint indices       = list_GLuint_alloc();

	enum {
		STATE_INITIAL = -1,
		STATE_POSITION,
		STATE_INDICES,
		STATE_NORMAL,
		STATE_TEX_COORD,
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
				debug_error("Failed to read token at '%s'", file_path);
				assert(0);
			}
		}

		// TODO The order in which a buffer is loaded from the lmod file matters.
		// need to change the importer so that it can handle parsing the buffers
		// in any order.

		if (strcmp(token, "vertex_indices:") == 0 || state == STATE_INDICES) {
			if (state != STATE_INDICES)
				c += sizeof("vertex_indices:");

			state = STATE_INDICES;

			GLuint index;
			int num_tokens = sscanf(c, "%u", &index);
			if (num_tokens != 1) {
				debug_error("Failed to read vertex_indices at '%s'", file_path);
				assert(0);
			}
			while(*c != ' ' && *c != '\0') { c++; } // skip the rest of the number

			//debug_log("%u ", index);
			list_GLuint_add(&indices, index);
		}

		if (strcmp(token, "vertex_texture_coordinates:") == 0 || state == STATE_TEX_COORD) {
			if (state != STATE_TEX_COORD)
				c += sizeof("vertex_texture_coordinates:");

			state = STATE_TEX_COORD;

			vector2_t tex_coord = {0};
			int num_tokens = sscanf(c, "%f %f", &tex_coord.x, &tex_coord.y);
			if (num_tokens != 2) {
				debug_error("Failed to read vertex_texture_coordinates at '%s'", file_path);
				assert(0);
			}

			list_vector2_t_add(&tex_coords, tex_coord);
			//vector2_print(tex_coords.array[tex_coords.length-1], "tex coord");
			while (*c != '\n' && *c != '\0') { c++; } 
		}

		if (strcmp(token, "vertex_normals:") == 0 || state == STATE_NORMAL) {
			if (state != STATE_NORMAL)
				c += sizeof("vertex_normals:");

			state = STATE_NORMAL;

			vector3_t normal = {0};
			int num_tokens = sscanf(c, "%f %f %f", &normal.x, &normal.y, &normal.z);
			if (num_tokens != 3) {
				debug_error("Failed to read vertex_normals at '%s'", file_path);
				assert(0);
			}

			//vector3_print(normal, "normal");
			list_vector3_t_add(&normals, normal);
			while (*c != '\n' && *c != '\0') { c++; } 
		}

		if (strcmp(token, "vertex_positions:") == 0 || state == STATE_POSITION) {
			if (state != STATE_POSITION)
				c += sizeof("vertex_positions:");

			state = STATE_POSITION;

			vector3_t position = {0};
			int num_tokens = sscanf(c, "%f %f %f", &position.x, &position.y, &position.z);
			if (num_tokens != 3) {
				debug_error("Failed to read vertex_positions at '%s'", file_path);
				assert(0);
			}

			//vector3_print(position, "position");
			list_vector3_t_add(&positions, position);
			while (*c != '\n' && *c != '\0') { c++; } 
		}
	}

	file_buffer_free(fb);

	list_vertex_t vertices = list_vertex_t_alloc();
	for(size_t i = 0; i < positions.length; i++) {
		vertex_t vertex = {0};
		if (positions.length > 0)
		vertex.position = positions.array[i];
		if (normals.length > 0)
		vertex.normal   = normals.array[i];
		if (tex_coords.length > 0)
		vertex.texCoord = tex_coords.array[i];
		list_vertex_t_add(&vertices, vertex);
	}

	list_vector3_t_free(&positions);
	list_vector3_t_free(&normals);
	list_vector2_t_free(&tex_coords);

	mesh_t mesh = lite_engine_gl_mesh_alloc(vertices, indices);
	return mesh;
}

void lite_engine_gl_mesh_free(mesh_t *mesh) {
	list_vertex_t_free(&mesh->vertices);
	list_GLuint_free(&mesh->indices);
}
// object lists to keep stuff hot on the cache
ui64                           internal_gl_num_entities;
camera_t                      *internal_gl_active_camera = NULL;

static char *internal_prefer_window_title         = "Game Window";
static ui16  internal_prefer_window_size_x        = 640;
static ui16  internal_prefer_window_size_y        = 480;
static ui16  internal_prefer_window_position_x    = 0;
static ui16  internal_prefer_window_position_y    = 0;
static ui8   internal_prefer_window_always_on_top = 0;
static ui8   internal_prefer_window_fullscreen    = 0;

static material_t    test_material;
static mesh_t        test_mesh;
static transform_t   test_transform;
static point_light_t test_light;
static transform_t   test_light_transform;
static camera_t      test_camera;

void lite_engine_gl_set_prefer_window_title(char *title) {
	internal_prefer_window_title = title;
}

void lite_engine_gl_set_prefer_window_size_x(ui16 size_x) {
	internal_prefer_window_size_x = size_x;
}

void lite_engine_gl_set_prefer_window_size_y(ui16 size_y) {
	internal_prefer_window_size_y = size_y;
}

void lite_engine_gl_set_prefer_window_position_x(ui16 pos_x) {
	internal_prefer_window_position_x = pos_x;
}

void lite_engine_gl_set_prefer_window_position_y(ui16 pos_y) {
	internal_prefer_window_position_y = pos_y;
}

void lite_engine_gl_set_prefer_window_always_on_top(ui8 always_on_top) {
	internal_prefer_window_always_on_top = always_on_top;
}

void lite_engine_gl_set_prefer_window_fullscreen(ui8 fullscreen) {
	internal_prefer_window_fullscreen = fullscreen;
}

typedef struct {
	GLFWwindow *window;
	char       *window_title;	
	ui16        window_size_x;
	ui16        window_size_y;
	ui16        window_position_x;
	ui16        window_position_y;
	ui8         window_always_on_top;
	ui8         window_fullscreen;
} opengl_context_t;

opengl_context_t *internal_gl_context = NULL;

void APIENTRY glDebugOutput(const GLenum source, const GLenum type,
		const unsigned int id, const GLenum severity,
		const GLsizei length, const char *message,
		const void *userParam) {
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
		case GL_DEBUG_TYPE_ERROR: {
			printf("Type: Error");
		} break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
			printf("Type: Deprecated Behaviour");
		} break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
			printf("Type: Undefined Behaviour");
		} break;
		case GL_DEBUG_TYPE_PORTABILITY: {
			printf("Type: Portability");
		} break;
		case GL_DEBUG_TYPE_PERFORMANCE: {
			printf("Type: Performance");
		} break;
		case GL_DEBUG_TYPE_MARKER: {
			printf("Type: Marker");
		} break;
		case GL_DEBUG_TYPE_PUSH_GROUP: {
			printf("Type: Push Group");
		} break;
		case GL_DEBUG_TYPE_POP_GROUP: {
			printf("Type: Pop Group");
		} break;
		case GL_DEBUG_TYPE_OTHER: {
			printf("Type: Other");
		} break;
	}
	printf("\n");

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: {
			printf("Severity: high");
		} break;
		case GL_DEBUG_SEVERITY_MEDIUM: {
			printf("Severity: medium");
		} break;
		case GL_DEBUG_SEVERITY_LOW: {
			printf("Severity: low");
		} break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: {
			printf("Severity: notification");
		} break;
	}
	printf("\n\n");
}

void error_callback(const int error, const char *description) {
	(void)error;
	debug_error("%s", description);
}

void framebuffer_size_callback(
		GLFWwindow *window,
		const int width,
		const int height) {
	(void)window;
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, const int key, const int scancode,
		const int action, const int mods) {
	(void)scancode;
	(void)mods;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void lite_engine_gl_start(void) {
	debug_log("initializing OpenGL renderer.");

	internal_gl_context                       = calloc(sizeof(*internal_gl_context), 1);
	internal_gl_context->window_title         = internal_prefer_window_title;
	internal_gl_context->window_size_x        = internal_prefer_window_size_x;
	internal_gl_context->window_size_y        = internal_prefer_window_size_y;
	internal_gl_context->window_position_x    = internal_prefer_window_position_x; 
	internal_gl_context->window_position_y    = internal_prefer_window_position_y;
	internal_gl_context->window_always_on_top = internal_prefer_window_always_on_top; 
	internal_gl_context->window_fullscreen    = internal_prefer_window_fullscreen; 

	if (!glfwInit()) {
		debug_error("Failed to initialize GLFW");
	}

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	if (internal_gl_context->window_always_on_top) {
		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	}

	if (internal_gl_context->window_fullscreen) {
		internal_gl_context->window = glfwCreateWindow(
				internal_gl_context->window_size_x, 
				internal_gl_context->window_size_y,
				internal_gl_context->window_title, 
				glfwGetPrimaryMonitor(), NULL);
	} else {
		internal_gl_context->window = glfwCreateWindow(
				internal_gl_context->window_size_x, 
				internal_gl_context->window_size_y,
				internal_gl_context->window_title, 
				NULL, NULL);
	}

	assert(internal_gl_context->window != NULL);

	glfwSetWindowPos(internal_gl_context->window, 
			internal_gl_context->window_position_x,
			internal_gl_context->window_position_y);
	glfwShowWindow(internal_gl_context->window);
	glfwMakeContextCurrent(internal_gl_context->window);
	glfwSetKeyCallback(internal_gl_context->window, key_callback);
	glfwSetFramebufferSizeCallback(
			internal_gl_context->window, 
			framebuffer_size_callback);
	glfwSetInputMode(internal_gl_context->window, 
			GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSwapInterval(0);

	if (!gladLoadGL()) {
		debug_error("Failed to initialize GLAD");
	}

	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
		debug_error("Failed to set debug context flag");
	} else {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, 
				GL_DONT_CARE, 0, NULL, GL_TRUE);
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	test_camera = (camera_t) {
		.transform.position    = (vector3_t){ 0.0, 0.0, -30.0 },
		.transform.rotation    = quaternion_identity(),
		.transform.scale       = vector3_one(1.0),
		.projection            = matrix4_identity(),
	};

	lite_engine_gl_set_active_camera(&test_camera);

	test_light_transform = (transform_t) {
		.position = vector3_right(100.0),
		.rotation = quaternion_from_euler(vector3_up(PI)),
		.scale    = vector3_one(1.0),
	};

	test_light = (point_light_t) {
		.diffuse = vector3_one(0.8f),
		.specular = vector3_one(1.0f),
		.constant = 1.0f,
		.linear = 0.09f,
		.quadratic = 0.0032f,
	};
		
	test_material = (material_t) {
		.shader = lite_engine_gl_shader_create(
				"res/shaders/phong_diffuse_vertex.glsl",
				"res/shaders/phong_diffuse_fragment.glsl"),
		.diffuseMap = lite_engine_gl_texture_create("res/textures/test.png"),
	};

	test_mesh = lite_engine_gl_mesh_lmod_alloc("res/models/untitled.lmod");

	test_transform = (transform_t) {
		.position = vector3_zero(),
		.rotation = quaternion_from_euler(vector3_up(PI)),
		.scale    = vector3_one(1.0),
	};

	debug_log("OpenGL renderer initialized successfuly");
}

void lite_engine_gl_render(void) {
	//debug_log("rendering...");
#if 1 // debugging input to exit
	if (glfwGetKey(internal_gl_context->window, GLFW_KEY_ESCAPE))
		lite_engine_stop();
#endif

	{ // projection
		int window_size_x;
		int window_size_y;
		glfwGetWindowSize(
				internal_gl_context->window, 
				&window_size_x,
				&window_size_y);
		float aspect = (float)internal_gl_context->window_size_x /
			(float)internal_gl_context->window_size_y;
		internal_gl_active_camera->projection =
			matrix4_perspective(deg2rad(60), aspect, 0.0001f, 1000.0f);
		internal_gl_active_camera->transform.matrix = 
			matrix4_identity();
		lite_engine_gl_transform_calculate_view_matrix(
				&internal_gl_active_camera->transform);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	test_transform.rotation = quaternion_multiply(test_transform.rotation,
			quaternion_from_euler(vector3_one(lite_engine_gl_get_time_delta())));
 
	lite_engine_gl_mesh_update(
			test_mesh,
			test_material,
			test_transform,
			test_light,
			test_light_transform);

	glfwSwapBuffers(internal_gl_context->window);
	glfwPollEvents();
}

void lite_engine_gl_set_active_camera(camera_t * camera) {
	internal_gl_active_camera = camera;
}

void lite_engine_gl_stop(void) {
}
