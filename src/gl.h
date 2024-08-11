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
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  bool invertFaces;
} mesh_t;

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
} cube_t;

DECLARE_LIST(cube_t)

typedef struct {
  transform_t transform;
matrix4_t projection;
  float lookSensitivity;
  float lastX;
  float lastY;
} camera_t;


DECLARE_LIST(mesh_t)

#define MESH_QUAD_NUM_VERTICES 4
#define MESH_QUAD_NUM_INDICES 6
#define MESH_CUBE_NUM_VERTICES 24
#define MESH_CUBE_NUM_INDICES 36

mesh_t mesh_alloc_cube(void);
mesh_t mesh_alloc_quad(void);
void mesh_free(mesh_t *m);
