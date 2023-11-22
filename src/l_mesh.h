#ifndef L_MESH_H
#define L_MESH_H

#include "stdlib.h"
#include "stdio.h"
#include "glad/glad.h"

typedef struct l_mesh l_mesh;
struct l_mesh {
	GLfloat* vertexData;
	GLuint numVertices;
	GLuint*  indexData;
	GLuint numIndices;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};
l_mesh l_mesh_create(
		GLfloat* vertexData, GLuint numVertices,
		GLuint* indexData, GLuint numIndices);
void l_mesh_draw(l_mesh* m);

#endif /*L_MESH_H*/
