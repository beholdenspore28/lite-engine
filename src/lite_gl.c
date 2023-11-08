#include "lite_gl.h"
#include "lite.h"
#include "HandmadeMath.h"

float _TEST_vertexData[_TEST_vertexDataLength] = {
	//front
	-0.5f, -0.5f, -0.5f, //bottom left
	0.2f,  0.2f,  0.2f, //color
	0.5f, -0.5f, -0.5f, //bottom right
	0.2f,  0.2f,  0.2f, //color
	-0.5f,  0.5f, -0.5f, //top left
	0.2f,  0.2f,  0.2f, //color
	0.5f,  0.5f, -0.5f, //top right
	0.2f,  0.2f,  0.2f, //color
						//back
	-0.5f, -0.5f,  0.5f, //bottom left
	0.4f,  0.4f,  0.4f, //color
	0.5f, -0.5f,  0.5f, //bottom right
	0.4f,  0.4f,  0.4f, //color
	-0.5f,  0.5f,  0.5f, //top left
	0.4f,  0.4f,  0.4f, //color
	0.5f,  0.5f,  0.5f, //top right
	0.4f,  0.4f,  0.4f, //color
};

GLuint _TEST_indexData[_TEST_indexDataLength] = {
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

//TODO remove these!
lite_mesh_t TESTmesh;
GLuint TESTshader = 0;
long double gTempTimer = 0.0;

void lite_mesh_setup(lite_mesh_t* mesh) {
	//TODO remove hard-coded data
	mesh->numIndices = _TEST_indexDataLength;
	mesh->numVertices = _TEST_vertexDataLength;
	mesh->indexData = _TEST_indexData;
	mesh->vertexData = _TEST_vertexData;

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

void _lite_glRenderMesh(lite_mesh_t* pMesh){
	glUseProgram(TESTshader);
	glBindVertexArray(pMesh->VAO);
	glDrawElements(
			GL_TRIANGLES,
			pMesh->numIndices,
			GL_UNSIGNED_INT,
			0);
	glUseProgram(0);
}

GLuint _lite_glCompileShader(
		GLuint type, const char* source){
	// printf("%s",source);
	//creation
	GLuint shader = 0;
	if (type == GL_VERTEX_SHADER){
		shader = glCreateShader(GL_VERTEX_SHADER);
	} else if (type == GL_FRAGMENT_SHADER) {
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	}

	//compilation
	glShaderSource(shader,1,&source,NULL);
	glCompileShader(shader);

	//Error check
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE){
		int length;
		glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
		char errorMessage[length];
		glGetShaderInfoLog(shader,length,&length,errorMessage);

		if (type == GL_VERTEX_SHADER){
			printf("%s\n", errorMessage);
			lite_printError("failed to compile vertex shader\n", 
					__FILE__, __LINE__);
		} else if (type == GL_FRAGMENT_SHADER){
			printf("%s\n", errorMessage);
			lite_printError("failed to compile fragment shader\n", 
					__FILE__, __LINE__);
		}
		glDeleteShader(shader);
	}
	return shader;
}

GLuint lite_glCreateShaderProgram(const char* vertsrc, const char* fragsrc){
	GLuint program = glCreateProgram();

	GLuint vertShader = _lite_glCompileShader(GL_VERTEX_SHADER, vertsrc);
	GLuint fragShader = _lite_glCompileShader(GL_FRAGMENT_SHADER, fragsrc);

	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glValidateProgram(program);

	return program;
}

GLuint lite_glPipeline_create() {
	printf("compiling shaders...\n");
	GLuint shaderProgram;
	blib_fileBuffer_t vertSourceFileBuffer = 
		blib_fileBuffer_read("res/shaders/vertex.glsl");
	blib_fileBuffer_t fragSourceFileBuffer = 
		blib_fileBuffer_read("res/shaders/fragment.glsl");

	if (vertSourceFileBuffer.error == true) {
		lite_printError("failed to read vertex shader", __FILE__, __LINE__);
	}
	if (fragSourceFileBuffer.error == true) {
		lite_printError("failed to read fragment shader", __FILE__, __LINE__);
	}

	const char* vertSourceString = vertSourceFileBuffer.text;
	const char* fragSourceString = fragSourceFileBuffer.text;

	// printf("%s\n\n%s\n", vertSourceString, fragSourceString);

	shaderProgram = lite_glCreateShaderProgram(
			vertSourceString,
			fragSourceString);

	blib_fileBuffer_close(vertSourceFileBuffer);
	blib_fileBuffer_close(fragSourceFileBuffer);

	printf("finished compiling shaders\n");
	return shaderProgram;
}

void _lite_glHandleSDLEvents(lite_engine_instance_t* instance){
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT){
			printf("[LITE-ENGINE] Quitting\n");
			instance->engineRunning = false;
		}
	}
}

void _lite_glPreRender(lite_engine_instance_t* instance){
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //render in wireframe mode
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//TODO move to object-specific render code
	glUseProgram(TESTshader);

	//model matrix
	HMM_Mat4 translationMat = HMM_Translate(
			(HMM_Vec3){.X=0.0f,.Y=0.0f,.Z=2.5f});

	HMM_Mat4 rotationMat = HMM_Rotate_LH(
			gTempTimer / HMM_RadToDeg,
			(HMM_Vec3){.X=0.5f,.Y=1.0f,.Z=0.0f});

	HMM_Mat4 scaleMat = HMM_Scale((HMM_Vec3){.X=1.0f,.Y=1.0f,.Z=1.0f});

	HMM_Mat4 modelMat = HMM_MulM4(translationMat, rotationMat);
	modelMat = HMM_MulM4(scaleMat, modelMat);

	GLint modelMatrixLocation = glGetUniformLocation(
			TESTshader, "u_modelMatrix");

	if (modelMatrixLocation >= 0) {
		glUniformMatrix4fv(
				modelMatrixLocation,
				1,
				GL_FALSE,
				&modelMat.Elements[0][0]);
	} else {
		lite_printError("failed to locate model matrix uniform", 
				__FILE__, __LINE__);
	}

	//projection matrix
	HMM_Mat4 projectionMat = HMM_Perspective_LH_NO(
			70, //fov
			(float)instance->screenWidth / instance->screenHeight, //aspect
			0.01f,    //near clip
			1000.0f); //far clip

	GLint projectionMatrixLocation = glGetUniformLocation(
			TESTshader, "u_projectionMatrix");

	if (projectionMatrixLocation >= 0) {
		glUniformMatrix4fv(
				projectionMatrixLocation,
				1,
				GL_FALSE,
				&projectionMat.Elements[0][0]);
	} else {
		lite_printError("failed to locate projection matrix uniform", 
				__FILE__, __LINE__);
	}
}

void _lite_glRenderFrame(lite_engine_instance_t* instance){
	// for (int i = 0; i < instance->renderListLength; i++){
	// 	_lite_glRenderMesh(&instance->renderList[i]);
	// }

	_lite_glRenderMesh(&TESTmesh);
}

void _lite_glUpdate(lite_engine_instance_t* instance){
	gTempTimer += 1.0f; //TODO this is TERRIBLE. remove pls
						// TODO : lock cursor to window and hide mouse
						// SDL_WarpMouseInWindow(
						// instance->SDLwindow, 
						// instance->screenWidth,instance->screenHeight);
						// SDL_SetRelativeMouseMode(SDL_TRUE);

	_lite_glHandleSDLEvents(instance);
	_lite_glPreRender(instance);
	_lite_glRenderFrame(instance);
	SDL_GL_SwapWindow(instance->SDLwindow);
}

void lite_glInitialize(lite_engine_instance_t* instance){
	//Set lite-engine function pointers
	instance->updateRenderer = &_lite_glUpdate;

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
			instance->windowTitle,
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
	glClearColor(0.1f,0.1f,0.2f,1.0f);
	glViewport(0,0,instance->screenWidth, instance->screenHeight);

	printf("==========================================================\n");
	printf("OPENGL INFO\n");
	printf("Vendor\t%s\n", glGetString(GL_VENDOR));
	printf("Renderer\t%s\n", glGetString(GL_RENDERER));
	printf("Version\t%s\n", glGetString(GL_VERSION));
	printf("Shading Language\t%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("==========================================================\n");
}
