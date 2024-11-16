#include "blib/blib.h"
#include <gl.h>
#include <stb_image.h>

DEFINE_LIST(GLint)
DEFINE_LIST(GLuint)
DEFINE_LIST(vertex_t)

// SHADER=====================================================================//

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
			fprintf(stderr, "failed to compile vertex shader\n%s\n", infoLog);
			exit(1);
		} else if (type == GL_FRAGMENT_SHADER) {
			fprintf(stderr, "failed to compile fragment shader\n%s\n", infoLog);
			exit(1);
		}
		glDeleteShader(shader);
	}
	return shader;
}

GLuint shader_create(const char *vertexShaderSourcePath,
		const char *fragmentShaderSourcePath) {
	printf("Attempting to load shaders '%s' and '%s'\n", vertexShaderSourcePath,
			fragmentShaderSourcePath);
	filebuffer_t vertSourceFileBuffer = filebuffer_read(vertexShaderSourcePath);
	filebuffer_t fragSourceFileBuffer = filebuffer_read(fragmentShaderSourcePath);

	if (vertSourceFileBuffer.error == true) {
		fprintf(stderr, "failed to read vertex shader");
		exit(1);
	}
	if (fragSourceFileBuffer.error == true) {
		fprintf(stderr, "failed to read fragment shader");
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
		fprintf(stderr, "\n\nfailed to link shader\n\n%s", infoLog);
	}

	glValidateProgram(program);

	filebuffer_close(vertSourceFileBuffer);
	filebuffer_close(fragSourceFileBuffer);

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

void shader_setUniformM4(GLuint shader, const char *uniformName, matrix4_t *m) {
	GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
	glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, &m->elements[0]);
}
// TRANSFORM==================================================================//

DEFINE_LIST(transform_t)
void transform_calculate_matrix(transform_t *t) {
  matrix4_t translation = matrix4_translate(t->position);
  matrix4_t rotation = quaternion_to_matrix4(t->rotation);
  matrix4_t scale = matrix4_scale(t->scale);
  t->matrix = matrix4_multiply(rotation, translation);
  t->matrix = matrix4_multiply(scale, t->matrix);
}

void transform_calculate_view_matrix(transform_t *t) {
  matrix4_t translation = matrix4_translate(vector3_negate(t->position));
  matrix4_t rotation = quaternion_to_matrix4(quaternion_conjugate(t->rotation));
  matrix4_t scale = matrix4_scale(t->scale);
  t->matrix = matrix4_multiply(translation, rotation);
  t->matrix = matrix4_multiply(scale, t->matrix);
}

vector3_t transform_basis_forward(transform_t t,
                                                float magnitude) {
  return vector3_rotate(vector3_forward(magnitude), t.rotation);
}

vector3_t transform_basis_up(transform_t t, float magnitude) {
  return vector3_rotate(vector3_up(magnitude), t.rotation);
}

vector3_t transform_basis_right(transform_t t, float magnitude) {
  return vector3_rotate(vector3_right(magnitude), t.rotation);
}

vector3_t transform_basis_back(transform_t t, float magnitude) {
  return vector3_rotate(vector3_back(magnitude), t.rotation);
}

vector3_t transform_basis_down(transform_t t, float magnitude) {
  return vector3_rotate(vector3_down(magnitude), t.rotation);
}

vector3_t transform_basis_left(transform_t t, float magnitude) {
  return vector3_rotate(vector3_left(magnitude), t.rotation);
}

// MESH=======================================================================//

DEFINE_LIST(mesh_t)

mesh_t mesh_alloc(vertex_t *vertices, GLuint *indices,
		GLuint numVertices, GLuint numIndices) {

	mesh_t m = {0};
	m.enabled = true;
	m.vertexCount = numVertices;
	m.indexCount = numIndices;

	glGenVertexArrays(1, &m.VAO);
	glGenBuffers(1, &m.VBO);
	glGenBuffers(1, &m.EBO);

	glBindVertexArray(m.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * numVertices, vertices,
			GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numIndices, indices,
			GL_STATIC_DRAW);

	GLuint vertStride = sizeof(vertex_t); // how many bytes per vertex?

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, position));
	glEnableVertexAttribArray(0);

	// texcoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, texCoord));
	glEnableVertexAttribArray(1);

	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertStride,
			(void *)offsetof(vertex_t, normal));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return m;
}

mesh_t mesh_alloc_quad(void) {
		return mesh_alloc(mesh_quad_vertices, mesh_quad_indices, 
				MESH_QUAD_NUM_VERTICES, MESH_QUAD_NUM_INDICES);
}
mesh_t mesh_alloc_cube(void) {
		return mesh_alloc(mesh_cube_vertices, mesh_cube_indices, 
				MESH_CUBE_NUM_VERTICES, MESH_CUBE_NUM_INDICES);
}

// TEXTURE====================================================================//

GLuint texture_create(const char *imageFile) {
	/*create texture*/
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	/*set parameters*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/*load texture data from file*/
	int width, height, numChannels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(imageFile, &width, &height, &numChannels, 0);

	/*error check*/
	if (data) {
		if (numChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
					GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else if (numChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
					GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	} else {
		fprintf(stderr, "failed to load texture from '%s'\n", imageFile);
	}

	/*cleanup*/
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}
