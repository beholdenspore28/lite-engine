#include "lite_gl_mesh.h"

lite_gl_mesh_t lite_gl_mesh_create(
		GLuint numIndices, GLuint numVertices, GLuint* indexData, 
		GLfloat* vertexData) {
	lite_gl_mesh_t m;
	/*TODO remove hard-coded data*/
	m.numIndices = numIndices;
	m.numVertices = numVertices;
	m.indexData = indexData;
	m.vertexData = vertexData;

	/*vertex array*/
	glGenVertexArrays(1, &m.VAO);
	glBindVertexArray(m.VAO);

	/*vertex buffer*/
	glGenBuffers(1, &m.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
	glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(GLfloat) * m.numVertices,
			m.vertexData,
			GL_STATIC_DRAW);

	/*index/element buffer*/
	glGenBuffers(1, &m.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
	glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			m.numIndices * sizeof(GLuint),
			m.indexData,
			GL_STATIC_DRAW);

	/*position attribute*/
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
			0,3,GL_FLOAT,GL_FALSE,
			sizeof(GLfloat) * 8, (GLvoid*)0);

	/*color attribute*/
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
			1,3,GL_FLOAT,GL_FALSE,
			sizeof(GLfloat) * 8,
			(GLvoid*)(sizeof(GLfloat) * 3));

	/*texture coord attribute*/
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
			2,2,GL_FLOAT,GL_FALSE,
			sizeof(GLfloat) * 8, 
			(GLvoid*)(sizeof(GLfloat)*6));

	/*cleanup*/
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	/*TODO add the new mesh to the drawing queue*/
	return m;
};

void lite_gl_mesh_render(lite_gl_mesh_t* pMesh){
	glBindVertexArray(pMesh->VAO);
	glDrawElements(
			GL_TRIANGLES,
			pMesh->numIndices,
			GL_UNSIGNED_INT,
			0);
	glUseProgram(0);
}
