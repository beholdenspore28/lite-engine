#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "blib/blib.h"
#include "blib/math3d.h"
#include "blib/file.h"
#include "blib/bmath.h"

DECLARE_LIST(GLint)
DECLARE_LIST(GLuint)

GLuint texture_create(const char *imageFile);

GLuint shader_create(const char *vertexShaderSourcePath,
                     const char *fragmentShaderSourcePath);
void shader_setUniformM4(GLuint shader, const char *uniformName, matrix4_t *m);
void shader_setUniformV3(GLuint shader, const char *uniformName, vector3_t v);
void shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f);
void shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i);

typedef enum {
  ENGINE_RENDERER_API_GL,
  ENGINE_RENDERER_API_NONE,
} engine_renderer_API_t;

typedef struct {
	vector3_t position;
	vector2_t texCoord;
	vector3_t normal;
} vertex_t;
DECLARE_LIST(vertex_t)

typedef struct {
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  list_vertex_t vertices;
  list_uint32_t indices;
  GLuint vertexCount;
  GLuint indexCount;
  bool invertFaces;
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

typedef struct {
	transform_t transform;
	material_t material;
	mesh_t mesh;
} quad_t;

typedef struct {
	transform_t transform;
	material_t material;
	mesh_t mesh;
} primitive_shape_t;
DECLARE_LIST(primitive_shape_t)

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

static vertex_t mesh_quad_vertices[MESH_QUAD_NUM_VERTICES] = {
	//positions         //tex	      //normal
	{ { 0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f }, { 0.0,  1.0,  0.0 } },// top right
	{ { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, { 0.0,  1.0,  0.0 } },// bottom right
	{ {-0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, { 0.0,  1.0,  0.0 } },// bottom left
	{ {-0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f }, { 0.0,  1.0,  0.0 } },// top left 
};

static unsigned int mesh_quad_indices[MESH_QUAD_NUM_INDICES] = {
	3, 1, 0,  // first Triangle
	3, 2, 1   // second Triangle
};

static unsigned int mesh_quad_indices_reversed[MESH_QUAD_NUM_INDICES] = {
	0, 1, 3,  // first Triangle
	1, 2, 3   // second Triangle
};

static vertex_t mesh_cube_vertices[MESH_CUBE_NUM_VERTICES] = {
	// position            //tex          //normal
	{ {-0.5,  0.5,  0.5 }, { 0.0,  1.0 }, { 0.0,  1.0,  0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 0.0,  0.0 }, { 0.0,  1.0,  0.0 } },
	{ { 0.5,  0.5, -0.5 }, { 1.0,  0.0 }, { 0.0,  1.0,  0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 1.0,  1.0 }, { 0.0,  1.0,  0.0 } },
	{ {-0.5,  0.5,  0.5 }, { 0.0,  1.0 }, {-1.0,  0.0,  0.0 } },
	{ {-0.5, -0.5,  0.5 }, { 0.0,  0.0 }, {-1.0,  0.0,  0.0 } },
	{ {-0.5, -0.5, -0.5 }, { 1.0,  0.0 }, {-1.0,  0.0,  0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 1.0,  1.0 }, {-1.0,  0.0,  0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 0.0,  1.0 }, { 0.0,  0.0, -1.0 } },
	{ {-0.5, -0.5, -0.5 }, { 0.0,  0.0 }, { 0.0,  0.0, -1.0 } },
	{ { 0.5, -0.5, -0.5 }, { 1.0,  0.0 }, { 0.0,  0.0, -1.0 } },
	{ { 0.5,  0.5, -0.5 }, { 1.0,  1.0 }, { 0.0,  0.0, -1.0 } },
	{ { 0.5,  0.5, -0.5 }, { 0.0,  1.0 }, { 1.0,  0.0,  0.0 } },
	{ { 0.5, -0.5, -0.5 }, { 0.0,  0.0 }, { 1.0,  0.0,  0.0 } },
	{ { 0.5, -0.5,  0.5 }, { 1.0,  0.0 }, { 1.0,  0.0,  0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 1.0,  1.0 }, { 1.0,  0.0,  0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 0.0,  1.0 }, { 0.0,  0.0,  1.0 } },
	{ { 0.5, -0.5,  0.5 }, { 0.0,  0.0 }, { 0.0,  0.0,  1.0 } },
	{ {-0.5, -0.5,  0.5 }, { 1.0,  0.0 }, { 0.0,  0.0,  1.0 } },
	{ {-0.5,  0.5,  0.5 }, { 1.0,  1.0 }, { 0.0,  0.0,  1.0 } },
	{ {-0.5, -0.5, -0.5 }, { 0.0,  1.0 }, { 0.0, -1.0,  0.0 } },
	{ {-0.5, -0.5,  0.5 }, { 0.0,  0.0 }, { 0.0, -1.0,  0.0 } },
	{ { 0.5, -0.5,  0.5 }, { 1.0,  0.0 }, { 0.0, -1.0,  0.0 } },
	{ { 0.5, -0.5, -0.5 }, { 1.0,  1.0 }, { 0.0, -1.0,  0.0 } },
};

static GLuint mesh_cube_indices[MESH_CUBE_NUM_INDICES] = {
	0,1,2,    0,2,3,    4,5,6,    4,6,7,    8,9,10,   8,10,11,
	12,13,14, 12,14,15, 16,17,18, 16,18,19, 20,21,22, 20,22,23,
};
static GLuint mesh_cube_indices_reversed[MESH_CUBE_NUM_INDICES] = {
	2,1,0,    3,2,0,    6,5,4,    7,6,4,    10,9,8,   11,10,8,
	14,13,12, 15,14,12, 18,17,16, 19,18,16, 22,21,20, 23,22,20,
};

mesh_t mesh_alloc(vertex_t *vertices, GLuint *indices,
		GLuint numVertices, GLuint numIndices);
mesh_t mesh_alloc_cube(bool invertFaces);
mesh_t mesh_alloc_quad(bool invertFaces);
