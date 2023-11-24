#include <stdio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "l_runtime.h"

// PRIMITIVE CUBE //===========================================================

#define _LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH 64
#define _LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH 36

static GLfloat _LITE_PRIMITIVE_CUBE_VERTEX_DATA[_LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH] = {
	/*front*/
	/*position        //color           //texcoord*/
	-0.5f,-0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 1.0f, 0.0f,/*bottom left*/
	0.5f,-0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 0.0f, 0.0f,/*bottom right*/
	-0.5f, 0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 1.0f, 1.0f,/*top left*/
	0.5f, 0.5f,-0.5f, 0.3f, 0.3f, 0.3f, 0.0f, 1.0f,/*top right*/

	/*back*/
	-0.5f,-0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f,/*bottom left*/
	0.5f,-0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,/*bottom right*/
	-0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, 1.0f,/*top left*/
	0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f,/*top right*/
};

static GLuint _LITE_PRIMITIVE_CUBE_INDEX_DATA[_LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH] = {
	/*front*/
	2,0,1, 3,2,1,
	/*right*/
	1,5,7, 7,3,1,
	/*back*/
	5,4,6, 5,6,7,
	/*left*/
	0,2,6, 0,6,4,
	/*top*/
	3,7,6, 2,3,6,
	/*bottom*/
	5,1,0, 0,4,5,
};

// SHADER //===================================================================

#include "blib_file.h"

static GLuint _lite_gl_compileShader(
		GLuint type, const char* source){
	/* printf("%s",source);*/
	/*creation*/
	GLuint shader = 0;
	if (type == GL_VERTEX_SHADER){
		shader = glCreateShader(GL_VERTEX_SHADER);
	} else if (type == GL_FRAGMENT_SHADER) {
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	}

	/*compilation*/
	glShaderSource(shader,1,&source,NULL);
	glCompileShader(shader);

	/*Error check*/
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE){
		int length;
		glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
		char errorMessage[length];
		glGetShaderInfoLog(shader,length,&length,errorMessage);

		if (type == GL_VERTEX_SHADER){
			// printf("%s\n", errorMessage);
			// lite_printError("failed to compile vertex shader\n", 
			// 		__FILE__, __LINE__);
		} else if (type == GL_FRAGMENT_SHADER){
			// printf("%s\n", errorMessage);
			// lite_printError("failed to compile fragment shader\n", 
			// 		__FILE__, __LINE__);
		}
		glDeleteShader(shader);
	}
	return shader;
}

static GLuint _lite_gl_createShaderProgram(
		const char* vertsrc, const char* fragsrc){

	GLuint program = glCreateProgram();
	GLuint vertShader = _lite_gl_compileShader(GL_VERTEX_SHADER, vertsrc);
	GLuint fragShader = _lite_gl_compileShader(GL_FRAGMENT_SHADER, fragsrc);

	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glValidateProgram(program);

	return program;
}

GLuint l_shader_create() {
	printf("compiling shaders...\n");
	GLuint shaderProgram;
	blib_fileBuffer_t vertSourceFileBuffer = 
		blib_fileBuffer_read("res/shaders/vertex.glsl");
	blib_fileBuffer_t fragSourceFileBuffer = 
		blib_fileBuffer_read("res/shaders/fragment.glsl");

	if (vertSourceFileBuffer.error == true) {
		// lite_printError("failed to read vertex shader", __FILE__, __LINE__);
	}
	if (fragSourceFileBuffer.error == true) {
		// lite_printError("failed to read fragment shader", __FILE__, __LINE__);
	}

	const char* vertSourceString = vertSourceFileBuffer.text;
	const char* fragSourceString = fragSourceFileBuffer.text;

	shaderProgram = _lite_gl_createShaderProgram(
			vertSourceString,
			fragSourceString);

	blib_fileBuffer_close(vertSourceFileBuffer);
	blib_fileBuffer_close(fragSourceFileBuffer);

	printf("finished compiling shaders\n");
	return shaderProgram;
}


// MESH //=====================================================================

typedef struct {
	GLfloat* vertexData;
	GLuint* indexData;
	GLuint numVertices;
	GLuint numIndices;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
} l_mesh;

l_mesh l_mesh_create(GLuint numIndices,GLuint numVertices,GLuint* indexData, GLfloat* vertexData){
	l_mesh m = (l_mesh){
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
}

void l_mesh_render(l_mesh* m) {
	glBindVertexArray(m->VAO);
	glDrawElements(GL_TRIANGLES,m->numIndices,GL_UNSIGNED_INT,0);
	glUseProgram(0);
}

int main (int argc, char* argv[]) {
	printf("Rev up those fryers!\n");
	l_runtime_data runtime = l_runtime_init();	

	GLuint shader = l_shader_create();
	
	l_mesh mesh = l_mesh_create(
			_LITE_PRIMITIVE_CUBE_INDEX_DATA_LENGTH,_LITE_PRIMITIVE_CUBE_VERTEX_DATA_LENGTH,
			_LITE_PRIMITIVE_CUBE_INDEX_DATA,_LITE_PRIMITIVE_CUBE_VERTEX_DATA
			);

	// l_transform transform = l_transform_create();
	
	// l_texture2D texture = l_texture2D_create(
	// 		"res/textures/test2.png");
	
	glUseProgram(shader);
	// glUniform1i(glGetUniformLocation(shader, "texture"), 0);
	// glUseProgram(0);

	while (!glfwWindowShouldClose(runtime.window)){
		runtime.frameStartTime = glfwGetTime();
		
		l_runtime_update(&runtime);
		l_mesh_render(&mesh);

		runtime.frameEndTime = glfwGetTime();
		runtime.deltaTime = runtime.frameEndTime - runtime.frameStartTime;
		// printf("frameend: %f framestart %f deltatime: %f\n",
		// 		runtime.frameEndTime, runtime.frameStartTime, runtime.deltaTime);
	}

	l_runtime_cleanup(&runtime);
	return 0;
}
