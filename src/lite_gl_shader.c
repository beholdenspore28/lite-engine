#include "lite_gl.h"

static GLuint shader_compile(GLuint type, const char *source) {
	/*creation*/
	GLuint shader = 0;
	if (type == GL_VERTEX_SHADER) {
		shader = glCreateShader(GL_VERTEX_SHADER);
	} else if (type == GL_FRAGMENT_SHADER) {
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	}

	/*compilation*/
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	/*Error check*/
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char infoLog[length];
		glGetShaderInfoLog(shader, length, &length, infoLog);

		if (type == GL_VERTEX_SHADER) {
			debug_error("Failed to compile vertex shader\n%s\n", infoLog);
			exit(1);
		} else if (type == GL_FRAGMENT_SHADER) {
			debug_error("Failed to compile fragment shader\n%s\n", infoLog);
			exit(1);
		}
		glDeleteShader(shader);
	}
	return shader;
}

GLuint shader_create(const char *vertexShaderSourcePath,
		const char *fragmentShaderSourcePath) {
	debug_log("Loading shaders from '%s' and '%s'", vertexShaderSourcePath,
			fragmentShaderSourcePath);
	file_buffer vertSourceFileBuffer = file_buffer_alloc(vertexShaderSourcePath);
	file_buffer fragSourceFileBuffer = file_buffer_alloc(fragmentShaderSourcePath);

	if (vertSourceFileBuffer.error == true) {
		debug_error("Failed to locate '%s'", vertexShaderSourcePath);
		exit(1);
	}
	if (fragSourceFileBuffer.error == true) {
		debug_error("Failed to locate '%s'", fragmentShaderSourcePath);
		exit(1);
	}

	const char *vertSourceString = vertSourceFileBuffer.text;
	const char *fragSourceString = fragSourceFileBuffer.text;

	GLuint program = glCreateProgram();
	GLuint vertShader = shader_compile(GL_VERTEX_SHADER, vertSourceString);
	GLuint fragShader = shader_compile(GL_FRAGMENT_SHADER, fragSourceString);

	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);

	glLinkProgram(program);

	GLint success;
	GLint length;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
	char infoLog[length];
	if (!success) {
		glGetProgramInfoLog(program, length, &length, infoLog);
		debug_error("Failed to link shader\n %s", infoLog);
	}

	glValidateProgram(program);

	file_buffer_free(vertSourceFileBuffer);
	file_buffer_free(fragSourceFileBuffer);

	return program;
}

void shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform1i(UniformLocation, i);
}

void shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform1f(UniformLocation, f);
}

void shader_setUniformV3(GLuint shader, const char *uniformName, vector3_t v) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform3f(UniformLocation, v.x, v.y, v.z);
}

void shader_setUniformV4(GLuint shader, const char *uniformName, vector4_t v) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform4f(UniformLocation, v.x, v.y, v.z, v.w);
}

void shader_setUniformM4(GLuint shader, const char *uniformName, matrix4_t *m) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, &m->elements[0]);
}
