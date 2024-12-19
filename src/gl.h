#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "blib/blib.h"
#include "blib/blib_math.h"
#include "blib/blib_math3d.h"
#include "blib/blib_file.h"

DECLARE_LIST(GLint)
DECLARE_LIST(GLuint)

void error_callback(const int error, const char *description);

void key_callback(
		GLFWwindow *window, 
		const int key, 
		const int scancode, 
		const int action, 
		const int mods);

void APIENTRY glDebugOutput(
	const GLenum source, 
	const GLenum type, 
	const unsigned int id, 
	const GLenum severity, 
	const GLsizei length, 
	const char *message, 
	const void *userParam);

GLuint texture_create(const char *imageFile);

GLuint shader_create(const char *vertexShaderSourcePath, const char *fragmentShaderSourcePath);
void shader_setUniformM4(GLuint shader, const char *uniformName, mat4_t *m);
void shader_setUniformV3(GLuint shader, const char *uniformName, vec3_t v);
void shader_setUniformV4(GLuint shader, const char* uniformName, const vec4_t color);
void shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f);
void shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i);

typedef struct pointLight {
	float constant;
	float linear;
	float quadratic;
	vec3_t diffuse;
	vec3_t specular;
} pointLight_t;
DECLARE_LIST(pointLight_t)

typedef struct {
	vec3_t position;
	vec2_t texCoord;
	vec3_t normal;
} vertex_t;
DECLARE_LIST(vertex_t)

typedef struct {
	bool          enabled;
	GLuint        VAO;
	GLuint        VBO;
	GLuint        EBO;
	list_vertex_t vertices;
	list_GLuint   indices;
	bool          use_wire_frame;
} mesh_t;
DECLARE_LIST(mesh_t)

typedef struct {
	GLuint shader;
	GLuint diffuseMap;
	GLuint specularMap;
} material_t;
	
typedef struct {
  mat4_t matrix;
  vec3_t position;
  quat_t rotation;
  vec3_t scale;
} transform_t;
DECLARE_LIST(transform_t)

void transform_calculate_matrix(transform_t *t);
void transform_calculate_view_matrix(transform_t *t);
vec3_t transform_basis_forward(transform_t t, float magnitude);
vec3_t transform_basis_up(transform_t t, float magnitude);
vec3_t transform_basis_right(transform_t t, float magnitude);
vec3_t transform_basis_back(transform_t t, float magnitude);
vec3_t transform_basis_down(transform_t t, float magnitude);
vec3_t transform_basis_left(transform_t t, float magnitude);

typedef struct {
  transform_t transform;
mat4_t projection;
  float lookSensitivity;
  float lastX;
  float lastY;
} camera_t;

#define MESH_QUAD_NUM_VERTICES 4
#define MESH_QUAD_NUM_INDICES 6
#define MESH_CUBE_NUM_VERTICES 24
#define MESH_CUBE_NUM_INDICES 36

mesh_t mesh_alloc(list_vertex_t vertices, list_GLuint indices);
mesh_t mesh_lmod_alloc(const char* file_path);
void mesh_free(mesh_t* mesh);
