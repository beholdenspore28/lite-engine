#include "blib_file.h"
#include "l_renderer_gl.h"

static GLuint _l_renderer_gl_shader_compile(GLuint type, const char *source) {
  printf("compiling shader %s\n", source);
  /* printf("%s",source);*/
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

static GLuint _l_renderer_gl_shader_createProgram(const char *vertsrc, const char *fragsrc) {

  GLuint program = glCreateProgram();
  GLuint vertShader = _l_renderer_gl_shader_compile(GL_VERTEX_SHADER, vertsrc);
  GLuint fragShader = _l_renderer_gl_shader_compile(GL_FRAGMENT_SHADER, fragsrc);

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

  return program;
}

GLuint l_renderer_gl_shader_create(const char *vertexShaderSourcePath, const char *fragmentShaderSourcePath) {
  printf("compiling shaders...");
  GLuint shaderProgram;
  blib_fileBuffer_t vertSourceFileBuffer = blib_fileBuffer_read(vertexShaderSourcePath);
  blib_fileBuffer_t fragSourceFileBuffer = blib_fileBuffer_read(fragmentShaderSourcePath);

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

  shaderProgram = _l_renderer_gl_shader_createProgram(vertSourceString, fragSourceString);

  blib_fileBuffer_close(vertSourceFileBuffer);
  blib_fileBuffer_close(fragSourceFileBuffer);

  printf("finished compiling shaders\n");
  return shaderProgram;
}

void l_renderer_gl_shader_useCamera(GLuint shader, l_renderer_gl_camera *cam) {
  glUseProgram(shader);
  l_renderer_gl_shader_setMat4Uniform(shader, "u_viewMatrix", &cam->viewMatrix);
  l_renderer_gl_shader_setMat4Uniform(shader, "u_projectionMatrix", &cam->projectionMatrix);
  glUseProgram(0);
}

void l_renderer_gl_shader_setUniform3f(GLuint shader, const char *uniformName, float a, float b, float c) {
  GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
  if (UniformLocation >= 0) {
    glUniform3f(UniformLocation, a, b, c);
  } else {
    fprintf(stderr, "failed to locate vec3 uniform. %s %i", __FILE__, __LINE__);
  }
}

void l_renderer_gl_shader_setMat4Uniform(GLuint shader, const char *uniformName, blib_mat4_t *m) {
  GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
  if (UniformLocation >= 0) {
    glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, &m->elements[0]);
  } else {
    fprintf(stderr, "failed to locate matrix uniform. %s %i", __FILE__, __LINE__);
  }
}
