#include "l_renderer_gl.h"

static GLfloat L_CUBE_VERT_DATA[L_CUBE_NUM_VERTS] = {
	//position         //tex      //normal
	-0.5,  0.5,  0.5,  0.0, 1.0,  0.0, -1.0,  0.0,
	-0.5,  0.5, -0.5,  0.0, 0.0,  0.0, -1.0,  0.0,
	 0.5,  0.5, -0.5,  1.0, 0.0,  0.0, -1.0,  0.0,
	 0.5,  0.5,  0.5,  1.0, 1.0,  0.0, -1.0,  0.0,
	-0.5,  0.5,  0.5,  0.0, 1.0, -0.1,  0.0,  0.0,
	-0.5, -0.5,  0.5,  0.0, 0.0, -0.1,  0.0,  0.0,
	-0.5, -0.5, -0.5,  1.0, 0.0, -0.1,  0.0,  0.0,
	-0.5,  0.5, -0.5,  1.0, 1.0, -0.1,  0.0,  0.0,
	-0.5,  0.5, -0.5,  0.0, 1.0,  0.0,  0.0, -1.0,
	-0.5, -0.5, -0.5,  0.0, 0.0,  0.0,  0.0, -1.0,
	 0.5, -0.5, -0.5,  1.0, 0.0,  0.0,  0.0, -1.0,
	 0.5,  0.5, -0.5,  1.0, 1.0,  0.0,  0.0, -1.0,
	 0.5,  0.5, -0.5,  0.0, 1.0,  1.0,  0.0,  0.0,
	 0.5, -0.5, -0.5,  0.0, 0.0,  1.0,  0.0,  0.0,
	 0.5, -0.5,  0.5,  1.0, 0.0,  1.0,  0.0,  0.0,
	 0.5,  0.5,  0.5,  1.0, 1.0,  1.0,  0.0,  0.0,
	 0.5,  0.5,  0.5,  0.0, 1.0,  0.0,  0.0,  1.0,
	 0.5, -0.5,  0.5,  0.0, 0.0,  0.0,  0.0,  1.0,
	-0.5, -0.5,  0.5,  1.0, 0.0,  0.0,  0.0,  1.0,
	-0.5,  0.5,  0.5,  1.0, 1.0,  0.0,  0.0,  1.0,
	-0.5, -0.5, -0.5,  0.0, 1.0,  0.0, -1.0,  0.0,
	-0.5, -0.5,  0.5,  0.0, 0.0,  0.0, -1.0,  0.0,
	 0.5, -0.5,  0.5,  1.0, 0.0,  0.0, -1.0,  0.0,
	 0.5, -0.5, -0.5,  1.0, 1.0,  0.0, -1.0,  0.0,
};

static GLuint L_CUBE_INDEX_DATA[L_CUBE_NUM_INDICES] = {
	0,1,2,     0,2,3,     4,5,6,     4,6,7,
	8,9,10,    8,10,11,   12,13,14,  12,14,15,
	16,17,18,  16,18,19,  20,21,22,  20,22,23,
};

// MESH //=====================================================================

l_renderer_gl_mesh l_renderer_gl_mesh_create(
		GLuint numIndices,
		GLuint numVertices,
		GLuint* indexData, 
		GLfloat* vertexData){
	l_renderer_gl_mesh m = (l_renderer_gl_mesh){
		.vertexData = vertexData, .indexData = indexData,
			.numVertices = numVertices, .numIndices = numIndices
	};

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

	GLfloat attribStride = sizeof(GLfloat) * 8;

	/*position attribute*/
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
			0,3,GL_FLOAT,GL_FALSE,
			attribStride, (GLvoid*)0);

	/*texture coord attribute*/
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
			1,2,GL_FLOAT,GL_FALSE,
			attribStride, 
			(GLvoid*)(sizeof(GLfloat)*3));

	/*normal vector attribute*/
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
			2,3,GL_FLOAT,GL_FALSE,
			attribStride, 
			(GLvoid*)(sizeof(GLfloat)*6));

	// /*color attribute*/
	// glEnableVertexAttribArray(3);
	// glVertexAttribPointer(
	// 		1,3,GL_FLOAT,GL_FALSE,
	// 		attribStride,
	// 		(GLvoid*)(sizeof(GLfloat) * 3));

	/*cleanup*/
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	// glDisableVertexAttribArray(3);

	/*TODO add the new mesh to the drawing queue*/
	return m;
}

l_renderer_gl_mesh l_renderer_gl_mesh_createCube(){
	return l_renderer_gl_mesh_create(
			L_CUBE_NUM_INDICES, L_CUBE_NUM_VERTS,
			L_CUBE_INDEX_DATA, L_CUBE_VERT_DATA
			);
}

void l_renderer_gl_mesh_render(l_renderer_gl_mesh* m) {
	glBindVertexArray(m->VAO);
	glDrawElements(GL_TRIANGLES,m->numIndices,GL_UNSIGNED_INT,0);
	glUseProgram(0);
	glBindVertexArray(0);
}
