#ifndef L_MESH_H
#define L_MESH_H

#include "glad/glad.h"

#define _LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH 64
#define _LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH 36

typedef struct {
	GLfloat* vertexData;
	GLuint* indexData;
	GLuint numVertices;
	GLuint numIndices;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
} l_mesh;

l_mesh l_mesh_create(GLuint numIndices,GLuint numVertices,GLuint* indexData, GLfloat* vertexData);
l_mesh l_mesh_createCube();
void l_mesh_render(l_mesh* m);

#endif /*L_MESH_H*/
