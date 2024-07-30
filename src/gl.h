#ifndef GL_H
#define GL_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "blib/b_3d.h"
#include "blib/b_file.h"
#include "blib/b_list.h"
#include "blib/b_math.h"

DECLARE_LIST(GLint)
DECLARE_LIST(GLuint)

GLuint texture_create(const char *imageFile);

GLuint shader_create(const char *vertexShaderSourcePath,
                     const char *fragmentShaderSourcePath);
void shader_setUniformM4(GLuint shader, const char *uniformName, matrix4_t *m);
void shader_setUniformV3(GLuint shader, const char *uniformName, vector3_t v);
void shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f);
void shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i);

typedef struct {
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
} mesh_t;

DECLARE_LIST(mesh_t)

#define MESH_QUAD_NUM_VERTS 4
#define MESH_QUAD_NUM_INDICES 6
#define MESH_CUBE_NUM_VERTICES 24
#define MESH_CUBE_NUM_INDICES 36

mesh_t mesh_alloc_cube(void);
mesh_t mesh_alloc_quad(void);
void mesh_free(mesh_t *m);

#endif
