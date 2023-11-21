#ifndef LITE_GL_MESH_H
#define LITE_GL_MESH_H

#include "glad/glad.h"

typedef struct lite_gl_mesh_t lite_gl_mesh_t;
struct lite_gl_mesh_t {
	GLfloat* vertexData;
	GLuint numVertices;
	GLuint* indexData;
	GLuint numIndices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};

lite_gl_mesh_t lite_gl_mesh_create(
		GLuint numIndices, GLuint numVertices, 
		GLuint* indexData, GLfloat* vertexData);
void lite_gl_mesh_render(lite_gl_mesh_t* pMesh);

#endif /*LITE_GL_MESH_H*/
