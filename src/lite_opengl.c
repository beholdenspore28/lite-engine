#include "stb_image.h"
#include "blib/blib.h"
#include "lite_opengl.h"

#include <ctype.h>

DEFINE_LIST(GLint)
DEFINE_LIST(GLuint)
DEFINE_LIST(vertex_t)
DEFINE_LIST(pointLight_t)
DEFINE_LIST(transform_t)
DEFINE_LIST(mesh_t)

void error_callback(const int error, const char *description) {
	(void)error;
	fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow *window, const int key, const int scancode,
		const int action, const int mods) {
	(void)scancode;
	(void)mods;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void APIENTRY glDebugOutput(const GLenum source, const GLenum type,
		const unsigned int id, const GLenum severity,
		const GLsizei length, const char *message,
		const void *userParam) {
	(void)length;
	(void)userParam;

	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
		return;
	}

	printf("---------------\n");
	printf("Debug message (%d) %s\n", id, message);

	switch (source) {
		case GL_DEBUG_SOURCE_API:
			printf("Source: API");
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			printf("Source: Window System");
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			printf("Source: Shader Compiler");
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			printf("Source: Third Party");
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			printf("Source: Application");
			break;
		case GL_DEBUG_SOURCE_OTHER:
			printf("Source: Other");
			break;
	}
	printf("\n");

	switch (type) {
		case GL_DEBUG_TYPE_ERROR: {
			printf("Type: Error");
		} break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
			printf("Type: Deprecated Behaviour");
		} break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
			printf("Type: Undefined Behaviour");
		} break;
		case GL_DEBUG_TYPE_PORTABILITY: {
			printf("Type: Portability");
		} break;
		case GL_DEBUG_TYPE_PERFORMANCE: {
			printf("Type: Performance");
		} break;
		case GL_DEBUG_TYPE_MARKER: {
			printf("Type: Marker");
		} break;
		case GL_DEBUG_TYPE_PUSH_GROUP: {
			printf("Type: Push Group");
		} break;
		case GL_DEBUG_TYPE_POP_GROUP: {
			printf("Type: Pop Group");
		} break;
		case GL_DEBUG_TYPE_OTHER: {
			printf("Type: Other");
		} break;
	}
	printf("\n");

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: {
			printf("Severity: high");
		} break;
		case GL_DEBUG_SEVERITY_MEDIUM: {
			printf("Severity: medium");
		} break;
		case GL_DEBUG_SEVERITY_LOW: {
			printf("Severity: low");
		} break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: {
			printf("Severity: notification");
		} break;
	}
	printf("\n\n");
}

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
  file_buffer vertSourceFileBuffer = file_buffer_alloc(vertexShaderSourcePath);
  file_buffer fragSourceFileBuffer = file_buffer_alloc(fragmentShaderSourcePath);

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
// TRANSFORM==================================================================//

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

vector3_t transform_basis_forward(transform_t t, float magnitude) {
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

mesh_t mesh_alloc(list_vertex_t vertices, list_GLuint indices) {
  mesh_t m   = {0};
  m.enabled  = true;
  m.vertices = vertices;
  m.indices  = indices;

  glGenVertexArrays(1, &m.VAO);
  glGenBuffers(1, &m.VBO);
  glGenBuffers(1, &m.EBO);

  glBindVertexArray(m.VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * vertices.length, vertices.array,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.length, indices.array,
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

mesh_t mesh_lmod_alloc(const char* file_path) {
	file_buffer fb = file_buffer_alloc(file_path);
	if (fb.error) {
		fprintf(stderr, "\nfailed to open .lmod file at '%s' did you specity the correct path?\n", file_path);
		assert(0);
	}

	list_vector3_t normals   = list_vector3_t_alloc();
	list_vector3_t positions = list_vector3_t_alloc();
	list_GLuint indices   = list_GLuint_alloc();

	enum {
		STATE_INITIAL = -1,
		STATE_POSITION,
		STATE_INDICES,
		STATE_NORMAL,
	};
	uint8_t state = STATE_INITIAL;

	for(char *c = fb.text; c < fb.text+fb.length; c++) {

		if (isspace(*c)) {
			continue;
		} else if (*c == '#') {
			while(*c != '\n' && *c != '\0') { c++; }
			continue;
		}

		char token[128];
		{
			int num_tokens = sscanf(c, "%s", token);
			if (num_tokens != 1) {
				fprintf(stderr, "\n[LMOD_PARSE] failed to read token at %s\n", file_path);
				assert(0);
			}
		}

		if (strcmp(token, "vertex_indices:") == 0 || state == STATE_INDICES) {
			if (state != STATE_INDICES)
				c += sizeof("vertex_indices:");

			state = STATE_INDICES;

			GLuint index;
			int num_tokens = sscanf(c, "%u", &index);
			if (num_tokens != 1) {
				fprintf(stderr, "\n[LMOD_PARSE] failed to read vertex_indices at %s\n", file_path);
				assert(0);
			}
			while(*c != ' ' && *c != '\0') { c++; } // skip the rest of the number

			//printf("%u ", index);
			list_GLuint_add(&indices, index);
		}

		if (strcmp(token, "vertex_normals:") == 0 || state == STATE_NORMAL) {
			if (state != STATE_NORMAL)
				c += sizeof("vertex_normals:");

			state = STATE_NORMAL;

			vector3_t normal = {0};
			int num_tokens = sscanf(c, "%f %f %f", &normal.x, &normal.y, &normal.z);
			if (num_tokens != 3) {
				fprintf(stderr, "\n[LMOD_PARSE] failed to read vertex_normals at %s\n", file_path);
				assert(0);
			}

			//vector3_print(normal, "normal");
			list_vector3_t_add(&normals, normal);
			while (*c != '\n' && *c != '\0') { c++; } 
		}

		if (strcmp(token, "vertex_positions:") == 0 || state == STATE_POSITION) {
			if (state != STATE_POSITION)
				c += sizeof("vertex_positions:");

			state = STATE_POSITION;

			vector3_t position = {0};
			int num_tokens = sscanf(c, "%f %f %f\n", &position.x, &position.y, &position.z);
			if (num_tokens != 3) {
				fprintf(stderr, "\n[LMOD_PARSE] failed to read vertex_positions at %s\n", file_path);
				assert(0);
			}

			//vector3_print(position, "position");
			list_vector3_t_add(&positions, position);
			while (*c != '\n' && *c != '\0') { c++; } 
		}
	}

	file_buffer_free(fb);

	list_vertex_t vertices = list_vertex_t_alloc();
	for(size_t i = 0; i < positions.length; i++) {
		vertex_t vertex = {0};
		vertex.position = positions.array[i];
		vertex.normal   = normals.array[i];
		list_vertex_t_add(&vertices, vertex);
	}
	
	list_vector3_t_free(&positions);
	list_vector3_t_free(&normals);

	mesh_t mesh = mesh_alloc(vertices, indices);
	return mesh;
}

void mesh_free(mesh_t* mesh) {
	list_vertex_t_free(&mesh->vertices);
	list_GLuint_free(&mesh->indices);
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