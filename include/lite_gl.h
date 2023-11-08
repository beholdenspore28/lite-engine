#ifndef LITE_GL_H
#define LITE_GL_H

#include <SDL2/SDL.h>
#include "glad/glad.h"

#include "lite.h"
#include "blib_file.h"
#include "HandmadeMath.h"

typedef struct lite_gl_transform_t lite_gl_transform_t;
struct lite_gl_transform_t {
	HMM_Vec3 position;
	HMM_Vec3 eulerAngles;
	HMM_Vec3 scale;
};

typedef struct lite_gl_mesh_t lite_gl_mesh_t;
struct lite_gl_mesh_t {
	//vertex positions and attriibutes
	GLfloat* vertexData;
	//the total number of vertices in the mesh
	GLuint numVertices;
	//winding order data
	GLuint* indexData;
	//the total number of indices in this mesh
	GLuint numIndices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};

typedef struct lite_gl_gameObject_t lite_gl_gameObject_t;
struct lite_gl_gameObject_t {
	lite_gl_transform_t transform;
	lite_gl_mesh_t mesh;
	GLuint shader;
};

//TODO GET RID OF THESE!!
#define _TEST_vertexDataLength 48
#define _TEST_indexDataLength 36
extern float _TEST_vertexData[_TEST_vertexDataLength];
extern GLuint _TEST_indexData[_TEST_indexDataLength];
extern lite_gl_gameObject_t TESTgameObject;
extern float gTempTimer;

void lite_gl_initialize(lite_engine_instance_t* instance);
GLuint lite_gl_pipeline_create();
lite_gl_mesh_t lite_gl_mesh_create();
lite_gl_gameObject_t lite_gl_gameObject_create();

#endif  //LITE_GL_H
