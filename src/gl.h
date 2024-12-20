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
void shader_setUniformM4(GLuint shader, const char *uniformName, matrix4_t *m);
void shader_setUniformV3(GLuint shader, const char *uniformName, vector3_t v);
void shader_setUniformV4(GLuint shader, const char* uniformName, const vector4_t color);
void shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f);
void shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i);

typedef struct pointLight {
	float constant;
	float linear;
	float quadratic;
	vector3_t diffuse;
	vector3_t specular;
} pointLight_t;
DECLARE_LIST(pointLight_t)

typedef struct {
	vector3_t position;
	vector2_t texCoord;
	vector3_t normal;
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
  matrix4_t matrix;
  vector3_t position;
  quaternion_t rotation;
  vector3_t scale;
} transform_t;
DECLARE_LIST(transform_t)

void transform_calculate_matrix(transform_t *t);
void transform_calculate_view_matrix(transform_t *t);
vector3_t transform_basis_forward(transform_t t, float magnitude);
vector3_t transform_basis_up(transform_t t, float magnitude);
vector3_t transform_basis_right(transform_t t, float magnitude);
vector3_t transform_basis_back(transform_t t, float magnitude);
vector3_t transform_basis_down(transform_t t, float magnitude);
vector3_t transform_basis_left(transform_t t, float magnitude);

typedef struct {
  transform_t transform;
matrix4_t projection;
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
