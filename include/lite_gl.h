#ifndef LITE_GL_H
#define LITE_GL_H

#include <SDL2/SDL.h>
#include "glad/glad.h"

#include "lite.h"
#include "blib_file.h"


typedef struct lite_mesh_t lite_mesh_t;
struct lite_mesh_t {
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

//TODO GET RID OF THESE!!
#define _TEST_vertexDataLength 48
#define _TEST_indexDataLength 36
extern float _TEST_vertexData[_TEST_vertexDataLength];
extern GLuint _TEST_indexData[_TEST_indexDataLength];

//TODO remove these!
extern lite_mesh_t TESTmesh;
extern GLuint TESTshader;
extern long double gTempTimer;

void lite_mesh_setup(lite_mesh_t* mesh);
void _lite_glRenderMesh(lite_mesh_t* pMesh);
void _lite_glHandleSDLEvents(lite_engine_instance_t* instance);
void _lite_glPreRender(lite_engine_instance_t* instance);
void _lite_glRenderFrame(lite_engine_instance_t* instance);
void _lite_glUpdate(lite_engine_instance_t* instance);
void lite_glInitialize(lite_engine_instance_t* instance);
GLuint _lite_glCompileShader(GLuint type, const char* source);
GLuint lite_glCreateShaderProgram(const char* vertsrc, const char* fragsrc);
GLuint lite_glPipeline_create();

#endif  //LITE_GL_H
