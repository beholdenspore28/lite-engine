#include <gl.h>
#include <stb_image.h>
#include "blib/b_list.h"

DEFINE_LIST(GLint)
DEFINE_LIST(GLuint)

static void error_callback(int error, const char *description) {
  (void)error;
  fprintf(stderr, "Error: %s\n", description); {
}}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,int mods) {
  (void)scancode;
  (void)mods;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static void framebuffer_size_callback(GLFWwindow *window, int width,int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,GLenum severity, GLsizei length,const char *message, const void *userParam) {
  (void)length;
  (void)userParam;

  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204){
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
  case GL_DEBUG_TYPE_ERROR:
    printf("Type: Error");
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    printf("Type: Deprecated Behaviour");
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    printf("Type: Undefined Behaviour");
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    printf("Type: Portability");
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    printf("Type: Performance");
    break;
  case GL_DEBUG_TYPE_MARKER:
    printf("Type: Marker");
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    printf("Type: Push Group");
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    printf("Type: Pop Group");
    break;
  case GL_DEBUG_TYPE_OTHER:
    printf("Type: Other");
    break;
  }
  printf("\n");

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    printf("Severity: high");
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    printf("Severity: medium");
    break;
  case GL_DEBUG_SEVERITY_LOW:
    printf("Severity: low");
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    printf("Severity: notification");
    break;
  }
  printf("\n");
  printf("\n");
}

window window_create(void) {
  if (!glfwInit()){
    printf("[ERROR_GLFW] Failed to initialize GLFW");
  }
  
  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GLFW_TRUE); // comment to toggle debug mode

  GLFWwindow *glfwWindow = glfwCreateWindow(1280, 720, "Game window", NULL, NULL);
  if (!glfwWindow){
    printf("[ERROR_GLFW] Failed to create GLFW window\n");
  }
  
  glfwMakeContextCurrent(glfwWindow);
  glfwSetKeyCallback(glfwWindow, key_callback);
  glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGL()) {
    printf("[ERROR_GL] Failed to initialize GLAD\n");
  }
  
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
    printf("[ERROR_GL] Failed to set debug context flag\n");
  } else {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                          GL_TRUE);
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  int width, height;
  glfwGetFramebufferSize(glfwWindow, &width, &height);
  glViewport(0, 0, width, height);

  glfwSwapInterval(0);
  glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

  window window;
  window.glfwWindow = glfwWindow;
  return window;
}

//SHADER=====================================================================//

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
	printf("Attempting to load shaders '%s' and '%s'\n", vertexShaderSourcePath, fragmentShaderSourcePath);
  FileBuffer vertSourceFileBuffer = FileBuffer_read(vertexShaderSourcePath);
  FileBuffer fragSourceFileBuffer = FileBuffer_read(fragmentShaderSourcePath);

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

  FileBuffer_close(vertSourceFileBuffer);
  FileBuffer_close(fragSourceFileBuffer);

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

void shader_setUniformV3(GLuint shader, const char *uniformName, vec3 v) {
  GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
  glUniform3f(UniformLocation, v.x, v.y, v.z);
}

void shader_setUniformM4(GLuint shader, const char *uniformName, mat4 *m) {
  GLuint UniformLocation = glGetUniformLocation(shader, uniformName);
  glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, &m->elements[0]);
}

//MESH=======================================================================//

typedef struct {
	vec3 position;
	vec2 texCoord;
	vec3 normal;
} vertex_t;

// clang-format off

static vertex_t mesh_quadVertices[MESH_QUAD_NUM_VERTS] = {
	//positions         //tex	      //normal
	{ { 0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f }, { 0.0,  1.0,  0.0 } },// top right
	{ { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, { 0.0,  1.0,  0.0 } },// bottom right
	{ {-0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, { 0.0,  1.0,  0.0 } },// bottom left
	{ {-0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f }, { 0.0,  1.0,  0.0 } },// top left 
};

static unsigned int mesh_quadIndices[MESH_QUAD_NUM_INDICES] = {
	3, 1, 0,  // first Triangle
	3, 2, 1   // second Triangle
};

static vertex_t mesh_cubeVertices[MESH_CUBE_NUM_VERTICES] = {
   // position       //tex       //normal
  { {-0.5,  0.5,  0.5 }, { 0.0,  1.0 }, { 0.0,  1.0,  0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 0.0,  0.0 }, { 0.0,  1.0,  0.0 } },
	{ { 0.5,  0.5, -0.5 }, { 1.0,  0.0 }, { 0.0,  1.0,  0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 1.0,  1.0 }, { 0.0,  1.0,  0.0 } },
	{ {-0.5,  0.5,  0.5 }, { 0.0,  1.0 }, {-1.0,  0.0,  0.0 } },
	{ {-0.5, -0.5,  0.5 }, { 0.0,  0.0 }, {-1.0,  0.0,  0.0 } },
	{ {-0.5, -0.5, -0.5 }, { 1.0,  0.0 }, {-1.0,  0.0,  0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 1.0,  1.0 }, {-1.0,  0.0,  0.0 } },
	{ {-0.5,  0.5, -0.5 }, { 0.0,  1.0 }, { 0.0,  0.0, -1.0 } },
  { {-0.5, -0.5, -0.5 }, { 0.0,  0.0 }, { 0.0,  0.0, -1.0 } },
	{ { 0.5, -0.5, -0.5 }, { 1.0,  0.0 }, { 0.0,  0.0, -1.0 } },
	{ { 0.5,  0.5, -0.5 }, { 1.0,  1.0 }, { 0.0,  0.0, -1.0 } },
	{ { 0.5,  0.5, -0.5 }, { 0.0,  1.0 }, { 1.0,  0.0,  0.0 } },
	{ { 0.5, -0.5, -0.5 }, { 0.0,  0.0 }, { 1.0,  0.0,  0.0 } },
	{ { 0.5, -0.5,  0.5 }, { 1.0,  0.0 }, { 1.0,  0.0,  0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 1.0,  1.0 }, { 1.0,  0.0,  0.0 } },
	{ { 0.5,  0.5,  0.5 }, { 0.0,  1.0 }, { 0.0,  0.0,  1.0 } },
	{ { 0.5, -0.5,  0.5 }, { 0.0,  0.0 }, { 0.0,  0.0,  1.0 } },
  { {-0.5, -0.5,  0.5 }, { 1.0,  0.0 }, { 0.0,  0.0,  1.0 } },
	{ {-0.5,  0.5,  0.5 }, { 1.0,  1.0 }, { 0.0,  0.0,  1.0 } },
	{ {-0.5, -0.5, -0.5 }, { 0.0,  1.0 }, { 0.0, -1.0,  0.0 } },
	{ {-0.5, -0.5,  0.5 }, { 0.0,  0.0 }, { 0.0, -1.0,  0.0 } },
	{ { 0.5, -0.5,  0.5 }, { 1.0,  0.0 }, { 0.0, -1.0,  0.0 } },
	{ { 0.5, -0.5, -0.5 }, { 1.0,  1.0 }, { 0.0, -1.0,  0.0 } },
};

static GLuint mesh_cubeIndices[MESH_CUBE_NUM_INDICES] = {
    0,1,2,    0,2,3,    4,5,6,    4,6,7,    8,9,10,   8,10,11,
    12,13,14, 12,14,15, 16,17,18, 16,18,19, 20,21,22, 20,22,23,
};

// clang-format on

DEFINE_LIST(mesh)

void mesh_alloc(mesh* m, vertex_t* vertices, GLuint *indices,
    GLuint numVertices, GLuint numIndices) {

	if (!m->isInitialized) {
		m->VAOs = list_GLuint_alloc();
		m->VBOs = list_GLuint_alloc();
		m->EBOs = list_GLuint_alloc();
		m->isInitialized = 1;
	}

	GLuint VAO, VBO, EBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * numVertices, vertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numIndices, indices,
               GL_STATIC_DRAW);

  GLuint vertStride = sizeof(vertex_t); // how many bytes per vertex?

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertStride,
		(void*)offsetof(vertex_t, position));
  glEnableVertexAttribArray(0);

  // texcoord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertStride,
		(void*)offsetof(vertex_t,texCoord));
  glEnableVertexAttribArray(1);

  // normal attribute
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertStride,
		(void*)offsetof(vertex_t, normal));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

	list_GLuint_add(&m->VAOs, VAO);
	list_GLuint_add(&m->VBOs, VBO);
	list_GLuint_add(&m->EBOs, EBO);
}

void mesh_allocCube(mesh* m) {
	mesh_alloc(m, mesh_cubeVertices, mesh_cubeIndices, 
			MESH_CUBE_NUM_VERTICES, MESH_CUBE_NUM_INDICES);
}

void mesh_allocQuad(mesh* m) {
	mesh_alloc(m, mesh_quadVertices, mesh_quadIndices, 
			MESH_CUBE_NUM_VERTICES, MESH_QUAD_NUM_INDICES);
}

void mesh_free(mesh* m) {
	list_GLuint_free(&m->VAOs);
	list_GLuint_free(&m->VBOs);
	list_GLuint_free(&m->EBOs);
}

//TEXTURE====================================================================//

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
