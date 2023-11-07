#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include "glad/glad.h"
#include "HandmadeMath.h"

#include "blib_file.h"

#include <assimp/cimport.h> // C-api
#include <assimp/scene.h>   // output data structure
#include <assimp/postprocess.h> //post processing flags

/*BEGIN-Lite-Engine-Instance-------------------------------------------------*/

void lite_printError(
		const char* message, const char* file, unsigned int line){
	fprintf(stderr, "[ERROR] in file \"%s\" line# %i \t%s\n"
			, file, line, message);
	exit(1);
}

typedef enum { 
	LITE_RENDER_API_OPENGL, 
	LITE_RENDER_API_VULKAN, 
	LITE_RENDER_API_DIRECTX 
} lite_render_api;

typedef struct lite_engine_instance_t lite_engine_instance_t;
struct lite_engine_instance_t {
	lite_render_api renderApi;
	int screenWidth;
	int screenHeight;
	SDL_Window* SDLwindow;
	SDL_GLContext glContext;
	bool engineRunning;
	
	void (*update) (struct lite_engine_instance_t*);
};

static void _lite_glPreDraw(lite_engine_instance_t* instance){
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
}

static void _lite_glHandleSDLEvents(lite_engine_instance_t* instance){
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT){
			printf("[LITE-ENGINE] Quitting\n");
			instance->engineRunning = false;
		}
	}
}

static void _lite_glUpdate(lite_engine_instance_t* instance){
	// TODO : lock cursor to window and hide mouse
	// SDL_WarpMouseInWindow(
			// instance->SDLwindow, 
			// instance->screenWidth,instance->screenHeight);
	// SDL_SetRelativeMouseMode(SDL_TRUE);
	
	while (instance->engineRunning) {
		_lite_glHandleSDLEvents(instance);
		_lite_glPreDraw(instance);
		SDL_GL_SwapWindow(instance->SDLwindow);
	}
}

static void _lite_glInitialize(lite_engine_instance_t* instance){
	//Set lite-engine function pointers
	instance->update = &_lite_glUpdate;

	//set up SDL2
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		lite_printError("failed to initialize SDL2", __FILE__, __LINE__);
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);

	instance->SDLwindow = SDL_CreateWindow(
			"Game Window",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			instance->screenWidth,
			instance->screenHeight,
			SDL_WINDOW_OPENGL);

	if (instance->SDLwindow == NULL){
		lite_printError("failed to create SDL_Window!",
				__FILE__, __LINE__);
	}

	instance->glContext = SDL_GL_CreateContext(instance->SDLwindow);
	if (instance->glContext == NULL){
		lite_printError("failed to create opengl context",
				__FILE__, __LINE__);
	}
	
	//Set up glad
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)){
		lite_printError("failed to load GLAD",
				__FILE__,__LINE__);
	}
	
	//set up opengl
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f,0.3f,0.5f,1.0f);
	glViewport(0,0,instance->screenWidth, instance->screenHeight);
	
	printf("Vendor\t%s\n", glGetString(GL_VENDOR));
	printf("Renderer\t%s\n", glGetString(GL_RENDERER));
	printf("Version\t%s\n", glGetString(GL_VERSION));
	printf("Shading Language\t%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

lite_engine_instance_t lite_engine_instance_construct(
		lite_render_api renderApi, int screenWidth, int screenHeight) {
	lite_engine_instance_t instance;
	instance.screenWidth = screenWidth;
	instance.screenHeight = screenHeight;
	instance.engineRunning = true;
	instance.renderApi = renderApi;
	
	switch (instance.renderApi){
		case LITE_RENDER_API_OPENGL:
				_lite_glInitialize(&instance);	
			break;
		case LITE_RENDER_API_VULKAN:
				lite_printError("VULKAN api is not supported yet!", 
						__FILE__, __LINE__);
			break;
		case LITE_RENDER_API_DIRECTX:
				lite_printError("DIRECTX api is not supported yet!",
						__FILE__, __LINE__);
			break;
	}

	return instance;
}

/*END-Lite-Engine-Instance---------------------------------------------------*/

/*BEGIN-Mesh-----------------------------------------------------------------*/

#define _TEST_vertexDataLength 48
const float _TEST_vertexData[_TEST_vertexDataLength] = {
	//front
	-0.5f, -0.5f, -0.5f, //bottom left
	 1.0f,  0.0f,  0.0f, //color
	 0.5f, -0.5f, -0.5f, //bottom right
	 0.0f,  1.0f,  0.0f, //color
	-0.5f,  0.5f, -0.5f, //top left
	 0.0f,  0.0f,  1.0f, //color
	 0.5f,  0.5f, -0.5f, //top right
	 1.0f,  0.0f,  0.0f, //color
	//back
	-0.5f, -0.5f,  0.5f, //bottom left
	 1.0f,  0.0f,  0.0f, //color
	 0.5f, -0.5f,  0.5f, //bottom right
	 0.0f,  1.0f,  0.0f, //color
	-0.5f,  0.5f,  0.5f, //top left
	 0.0f,  0.0f,  1.0f, //color
	 0.5f,  0.5f,  0.5f, //top right
	 1.0f,  0.0f,  0.0f, //color
};

#define _TEST_indexDataLength 36
const GLuint _TEST_indexData[_TEST_indexDataLength] = {
	//front
	2,0,1, 3,2,1,
	//right
	1,5,7, 7,3,1,
	//back
	5,4,6, 5,6,7,
	//left
	0,2,6, 0,6,4,
	//top
	3,7,6, 2,3,6,
	//bottom
	5,1,0, 0,4,5,
};

typedef struct lite_mesh_t lite_mesh_t;
struct lite_mesh_t {
	//vertex positions and attriibutes
	float* vertexData;
	//the total number of vertices in the mesh
	unsigned int numVertices;
	//winding order data
	int* indexData;
	//the total number of indices in this mesh
	unsigned int numIndices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};

void lite_mesh_setup(lite_mesh_t* mesh) {
	//TODO remove hard-coded data
	mesh->numIndices = _TEST_indexDataLength;
	mesh->numVertices = _TEST_vertexDataLength;
	mesh->indexData = &_TEST_indexData;
	mesh->vertexData = &_TEST_vertexData;

	//vertex array
	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);

	//vertex buffer
	glGenBuffers(1, &mesh->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(GLfloat) * mesh->numVertices,
			mesh->vertexData,
			GL_STATIC_DRAW);

	//index/element buffer
	glGenBuffers(1, &mesh->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
	glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			mesh->numIndices * sizeof(GLuint),
			mesh->indexData,
			GL_STATIC_DRAW);
	
	//position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
			0,3,GL_FLOAT,GL_FALSE,
			sizeof(GLfloat) * 6, (GLvoid*)0);

	//color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
			1,3,GL_FLOAT,GL_FALSE,
			sizeof(GLfloat) * 6,
			(GLvoid*)(sizeof(GLfloat) * 3));

	//cleanup
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	//TODO add the new mesh to the drawing queue
};

/*END-Mesh-------------------------------------------------------------------*/

/*BEGIN-Test-----------------------------------------------------------------*/

int main(int argc, char** argv) {
	printf("\nRev up those fryers!\n\n");

	lite_engine_instance_t instance = 
		lite_engine_instance_construct(LITE_RENDER_API_OPENGL,640,480);

	lite_mesh_t mesh;
	lite_mesh_setup(&mesh);

	instance.update(&instance);

	return EXIT_SUCCESS;
}

/*END-Test-------------------------------------------------------------------*/
