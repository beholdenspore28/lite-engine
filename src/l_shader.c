#include "l_shader.h"

static GLuint _l_shader_compile(
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
			fprintf(stderr, "failed to compile vertex shader: %s\n", errorMessage);
		} else if (type == GL_FRAGMENT_SHADER){
			fprintf(stderr, "failed to compile fragment shader: %s\n", errorMessage);
		}
		glDeleteShader(shader);
	}
	return shader;
}

static GLuint _l_shader_createProgram(
		const char* vertsrc, const char* fragsrc){

	GLuint program = glCreateProgram();
	GLuint vertShader = _l_shader_compile(GL_VERTEX_SHADER, vertsrc);
	GLuint fragShader = _l_shader_compile(GL_FRAGMENT_SHADER, fragsrc);

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
		fprintf(stderr, "failed to read vertex shader %s, %i", __FILE__, __LINE__);
	}
	if (fragSourceFileBuffer.error == true) {
		fprintf(stderr, "failed to read fragment shader %s, %i", __FILE__, __LINE__);
	}

	const char* vertSourceString = vertSourceFileBuffer.text;
	const char* fragSourceString = fragSourceFileBuffer.text;

	shaderProgram = _l_shader_createProgram(
			vertSourceString,
			fragSourceString);

	blib_fileBuffer_close(vertSourceFileBuffer);
	blib_fileBuffer_close(fragSourceFileBuffer);

	printf("finished compiling shaders\n");
	return shaderProgram;
}
