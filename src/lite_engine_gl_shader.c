#include "lite_engine_gl.h"

static GLuint internal_shader_compile(GLuint type, const char *source) {
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

GLuint lite_engine_gl_shader_create(
		const char *vertex_shader_file_path,
		const char *fragment_shader_file_path) {

	debug_log("Loading shaders from '%s' and '%s'", 
			vertex_shader_file_path,
			fragment_shader_file_path);

	file_buffer vertex_source_string = file_buffer_alloc(vertex_shader_file_path);
	file_buffer fragment_source_string = file_buffer_alloc(fragment_shader_file_path);

	if (vertex_source_string.error == 1) {
		debug_error(
				"Failed to locate '%s'", 
				vertex_shader_file_path);
		exit(1);
	}
	if (fragment_source_string.error == 1) {
		debug_error(
				"Failed to locate '%s'", 
				fragment_shader_file_path);
		exit(1);
	}

	const char *vertSourceString = vertex_source_string.text;
	const char *fragSourceString = fragment_source_string.text;

	GLuint program = glCreateProgram();

	GLuint vertShader = internal_shader_compile(
			GL_VERTEX_SHADER, 
			vertSourceString);

	GLuint fragShader = internal_shader_compile(
			GL_FRAGMENT_SHADER, 
			fragSourceString);

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

	file_buffer_free(vertex_source_string);
	file_buffer_free(fragment_source_string);

	return program;
}

void lite_engine_gl_shader_setUniformInt(GLuint shader, const char *uniformName, GLuint i) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform1i(UniformLocation, i);
}

void lite_engine_gl_shader_setUniformFloat(GLuint shader, const char *uniformName, GLfloat f) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform1f(UniformLocation, f);
}

void lite_engine_gl_shader_setUniformV3(GLuint shader, const char *uniformName, vector3_t v) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform3f(UniformLocation, v.x, v.y, v.z);
}

void lite_engine_gl_shader_setUniformV4(GLuint shader, const char *uniformName, vector4_t v) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniform4f(UniformLocation, v.x, v.y, v.z, v.w);
}

void lite_engine_gl_shader_setUniformM4(GLuint shader, const char *uniformName, matrix4_t *m) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, &m->elements[0]);
}
