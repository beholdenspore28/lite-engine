#ifndef GLECS_GL_H
#define GLECS_GL_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <blib/b_file.h>
#include <blib/b_math.h>
#include <blib/b_3d.h>

typedef struct {
  GLFWwindow *glfwWindow;
  int width;
  int height;
} window;

window window_create();

GLuint texture_create(const char *imageFile);

GLuint shader_create(const char *vertexShaderSourcePath,
                     const char *fragmentShaderSourcePath);
void shader_setUniformM4(GLuint shader, const char *uniformName, mat4 *m);
void shader_setUniformV3(GLuint shader, const char *uniformName, vec3 v);
void shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f);
void shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i);

#define MESH_QUAD_NUM_VERTS 4
#define MESH_QUAD_NUM_INDICES 6
#define MESH_CUBE_NUM_VERTICES 24
#define MESH_CUBE_NUM_INDICES 36
#define MESH_MAX_COUNT 1024

void mesh_createCube(GLuint index);
GLuint mesh_getVAO(GLuint index);
GLuint mesh_getVBO(GLuint index);
GLuint mesh_getEBO(GLuint index);

#endif
