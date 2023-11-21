#include "lite_gl.h"

float _TEST_vertexData[_TEST_vertexDataLength] = {
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

GLuint _TEST_indexData[_TEST_indexDataLength] = {
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

//TODO make a destroy texture func to clean this up
GLuint lite_gl_texture_create(const char* imageFile){
	/*create texture*/
	GLuint texture;
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);

	/*set parameters*/
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	/*load texture data from file*/
	int width, height, numChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(
			imageFile, &width, &height, &numChannels, 0);

	/*error check*/
	if (data){
		if (numChannels == 4){
			glTexImage2D(
					GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,
					GL_UNSIGNED_BYTE,data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else if (numChannels == 3){
			glTexImage2D(
					GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,
					GL_UNSIGNED_BYTE,data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	} else {
		lite_printError("failed to load texture", __FILE__, __LINE__);
	}

	/*cleanup*/
	stbi_image_free(data);

	return texture;
}

static blib_mat4_t _lite_gl_camera_GetViewMatrix(lite_gl_transform_t* t){
	/*translation*/
	blib_mat4_t translationMat = blib_mat4_translateVec3(t->position);

	/*rotation*/
	blib_mat4_t p = blib_mat4_rotate(t->eulerAngles.x, BLIB_VEC3F_RIGHT);
	blib_mat4_t y = blib_mat4_rotate(t->eulerAngles.y, BLIB_VEC3F_UP);
	blib_mat4_t r = blib_mat4_rotate(t->eulerAngles.z, BLIB_VEC3F_FORWARD);
	blib_mat4_t rotationMat = blib_mat4_multiply(blib_mat4_multiply(r, y), p); 

	/*scale*/
	blib_mat4_t scaleMat = blib_mat4_scale(t->scale);

	/*TRS = model matrix*/
	blib_mat4_t modelMat = blib_mat4_multiply(translationMat, rotationMat);
	modelMat = blib_mat4_multiply(scaleMat, modelMat);

	return modelMat;
}

void lite_gl_camera_setProjectionMatrix(lite_gl_camera_t* cam, float aspect) {
	cam->projectionMatrix = blib_mat4_perspective(
			blib_mathf_deg2rad(cam->fov),
			aspect,
			0.01f,    /*near clip*/
			1000.0f); /*far clip*/
}

lite_gl_camera_t lite_gl_camera_create(float fov) {
	lite_gl_camera_t cam;
	
	cam.transform = lite_gl_transform_create();
	cam.transform.position.z = 5.0f;
	cam.fov = fov;

	return cam;
}

//TODO use events to tell the camera to update its projection matrix when
//the screen resolution changes
static void _lite_gl_camera_update(lite_gl_camera_t* cam, 
		lite_engine_instance_t* instance) {
	cam->viewMatrix = _lite_gl_camera_GetViewMatrix(&cam->transform);
	lite_gl_camera_setProjectionMatrix(
			cam, (float)instance->screenWidth / (float)instance->screenHeight);
}

lite_gl_mesh_t lite_gl_mesh_create(
		GLuint numIndices, GLuint numVertices, GLuint* indexData, 
		GLfloat* vertexData) {
	lite_gl_mesh_t m;
	/*TODO remove hard-coded data*/
	m.numIndices = numIndices;
	m.numVertices = numVertices;
	m.indexData = indexData;
	m.vertexData = vertexData;

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
};

static void _lite_gl_mesh_render(lite_gl_mesh_t* pMesh){
	glBindVertexArray(pMesh->VAO);
	glDrawElements(
			GL_TRIANGLES,
			pMesh->numIndices,
			GL_UNSIGNED_INT,
			0);
	glUseProgram(0);
}

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

static GLuint lite_gl_createShaderProgram(
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

GLuint lite_gl_pipeline_create() {
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

	shaderProgram = lite_gl_createShaderProgram(
			vertSourceString,
			fragSourceString);

	blib_fileBuffer_close(vertSourceFileBuffer);
	blib_fileBuffer_close(fragSourceFileBuffer);

	printf("finished compiling shaders\n");
	return shaderProgram;
}

lite_gl_gameObject_t lite_gl_gameObject_create(){
	lite_gl_gameObject_t go;
	go.shader = lite_gl_pipeline_create();
	go.mesh = lite_gl_mesh_create(
			_TEST_indexDataLength, _TEST_vertexDataLength, 
			_TEST_indexData, _TEST_vertexData);
	go.texture = lite_gl_texture_create(
			"res/textures/test2.png");
	go.active = true;
	go.transform = lite_gl_transform_create();

	/*TODO texture glitch here*/
	/*DO NOT move this to update loop*/
	glUseProgram(go.shader);
	glUniform1i(glGetUniformLocation(go.shader, "texture"), 0);
	glUseProgram(0);

	return go;
}

static void _lite_gl_gameObject_update(
		lite_gl_gameObject_t* go, lite_engine_instance_t* instance){
	if (go->active == false) return;
	glUseProgram(go->shader);

	/*model matrix*/
	blib_mat4_t modelMat = lite_gl_transform_GetMatrix(
			&go->transform);

	GLint modelMatrixLocation = glGetUniformLocation(
			go->shader, "u_modelMatrix");

	if (modelMatrixLocation >= 0) {
		glUniformMatrix4fv(
				modelMatrixLocation,
				1,
				GL_FALSE,
				&modelMat.elements[0]);
	} else {
		lite_printError("failed to locate model matrix uniform", 
				__FILE__, __LINE__);
	}

	/*
	TODO cache projectionMatrixLocation in ram to prevent
	calling GetUniformLocation every frame
	*/

	/*projection matrix*/
	GLint projectionMatrixLocation = glGetUniformLocation(
			go->shader, "u_projectionMatrix");

	if (projectionMatrixLocation >= 0) {
		glUniformMatrix4fv(
				projectionMatrixLocation,
				1,
				GL_FALSE,
				&TESTcamera.projectionMatrix.elements[0]);
	} else {
		lite_printError("failed to locate projection matrix uniform", 
				__FILE__, __LINE__);
	}

	GLint viewMatrixLocation = glGetUniformLocation(
			go->shader, "u_viewMatrix");

	if (viewMatrixLocation >= 0) {
		glUniformMatrix4fv(
				viewMatrixLocation,
				1,
				GL_FALSE,
				&TESTcamera.viewMatrix.elements[0]);
	} else {
		lite_printError("failed to locate view matrix uniform", 
				__FILE__, __LINE__);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, go->texture);

	_lite_gl_mesh_render(&go->mesh);
}

static void _lite_gl_windowResize(lite_engine_instance_t* instance, Sint32 w, Sint32 h) {
	instance->screenHeight = h;
	instance->screenWidth = w;
	glViewport(0,0,instance->screenWidth,instance->screenHeight);
}

static void _lite_gl_handleSDLEvents(lite_engine_instance_t* instance){
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				printf("[LITE-ENGINE] Quitting\n");
				instance->engineRunning = false;
				break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					_lite_gl_windowResize(instance, e.window.data1, e.window.data2);
				}
				break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	/*quit button*/
	if (keyState[SDL_SCANCODE_ESCAPE]) {
		instance->engineRunning = false;
	}

	// /*rotate cube*/
	// float cubespeed = 10.0f * blib_mathf_deg2rad(10.0f) * instance->deltaTime;	
	// _lite_gl_transform_rotate(
	// 	&TESTgameObject.transform,
	// 	blib_vec3f_scale(BLIB_VEC3F_UP, cubespeed));	

	/*move camera*/
	/*TODO camera gets faster as you move*/
	inputVector = (blib_vec3f_t) { 
		.x = keyState[SDL_SCANCODE_A] - keyState[SDL_SCANCODE_D],
		.y = keyState[SDL_SCANCODE_LSHIFT] - keyState[SDL_SCANCODE_SPACE],
		.z = keyState[SDL_SCANCODE_S] - keyState[SDL_SCANCODE_W],
	};
	inputVector = blib_vec3f_scale(inputVector, instance->deltaTime * 15.0f);
	blib_vec3f_t* cameraPos = &TESTcamera.transform.position;

	blib_vec3f_t camForward = blib_vec3f_scale(
			lite_transform_getLocalForward(&TESTcamera.transform), 
			inputVector.z);
	blib_vec3f_t camUp = blib_vec3f_scale(
			lite_transform_getLocalUp(&TESTcamera.transform),
			inputVector.y);
	blib_vec3f_t camRight = blib_vec3f_scale(
		lite_transform_getLocalRight(&TESTcamera.transform),
			inputVector.x);
	/*
	printf("forward mag: %f\n", blib_vec3f_magnitude(camForward));
	printf("up mag: %f\n", blib_vec3f_magnitude(camUp));
	printf("right mag: %f\n", blib_vec3f_magnitude(camRight));
	*/

	*cameraPos = blib_vec3f_add(*cameraPos, camForward);
	*cameraPos = blib_vec3f_add(*cameraPos, camRight);
	*cameraPos = blib_vec3f_add(*cameraPos, camUp);

	/*rotate camera*/
	inputVector2 = (blib_vec3f_t) { 
		.x = keyState[SDL_SCANCODE_K] - keyState[SDL_SCANCODE_I],
		.y = keyState[SDL_SCANCODE_J] - keyState[SDL_SCANCODE_L],
		.z = 0.0f
		// .z = keyState[SDL_SCANCODE_O] - keyState[SDL_SCANCODE_U],
	};
	float speed = blib_mathf_deg2rad(100.0f) * instance->deltaTime;
	blib_vec3f_t rot = blib_vec3f_scale(inputVector2,speed);
	lite_gl_transform_rotate(&TESTcamera.transform, rot);

	/*reset button*/
	if (keyState[SDL_SCANCODE_R]) {
		TESTcamera.transform.position = blib_vec3f_scale(BLIB_VEC3F_FORWARD, 2.0f);
		TESTcamera.transform.eulerAngles = BLIB_VEC3F_ZERO;
		TESTgameObject.transform.position = BLIB_VEC3F_ZERO;
		TESTgameObject.transform.eulerAngles = BLIB_VEC3F_ZERO;
	}
	/*log stuff*/
	// printf("w%i h%i\n", instance->screenWidth, instance->screenHeight);
	// blib_mat4_printf(cam->viewMatrix, "viewMatrix");
	// printf("inputVector %f %f %f\n", 
	// 		inputVector.x, inputVector.y, inputVector.z);
	//
	// printf("inputVector2 %f %f %f\n", 
	// 		inputVector2.x, inputVector2.y, inputVector2.z);
	//
	// printf("cubePosition %f %f %f\n", 
	// 		TESTgameObject.transform.position.x, 
	// 		TESTgameObject.transform.position.y, 
	// 		TESTgameObject.transform.position.z);
	//
	// printf("cubeRotation %f %f %f\n", 
	// 		TESTgameObject.transform.eulerAngles.x, 
	// 		TESTgameObject.transform.eulerAngles.y, 
	// 		TESTgameObject.transform.eulerAngles.z);
	//
	// printf("cameraPosition %f %f %f\n", 
	// 		TESTcamera.transform.position.x, 
	// 		TESTcamera.transform.position.y, 
	// 		TESTcamera.transform.position.z);
	//
	// printf("cameraRotation %f %f %f\n", 
	// 		TESTcamera.transform.eulerAngles.x, 
	// 		TESTcamera.transform.eulerAngles.y, 
	// 		TESTcamera.transform.eulerAngles.z);
}

static void _lite_gl_renderFrame(lite_engine_instance_t* instance){
	_lite_gl_camera_update(&TESTcamera, instance);
	_lite_gl_gameObject_update(&TESTgameObject, instance);

	float distanceBetweenCubes = 1.0f;
	int cap = 100;
	float i = -cap;
	float j = -cap;
	static blib_vec2f_t point = (blib_vec2f_t){.x=2.0f, .y=1.0f };

	point = blib_vec2f_add(
			point, 
			blib_vec2f_scale(
				(blib_vec2f_t){.x=1.0f,.y=0.5f}, 
				4 * instance->deltaTime));

	for (i = 0; i < cap; i++){
		for (j = 0; j < cap; j++) {
			// TESTgameObject.transform.position = 
			// 	(blib_vec3f_t){
			// 		.x=i*distanceBetweenCubes,
			// 		.y=blib_noise_perlin2d(
			// 				i*0.25f + point.x, j*0.25f + point.y, 0.2f, 2) * 20.0f,
			// 		.z=j*distanceBetweenCubes};

			TESTgameObject.transform.position = 
				(blib_vec3f_t){
					.x=i*distanceBetweenCubes,
					.y=blib_noise_perlin2d(
							i*0.25f, j*0.25f, 0.2f, 2) * 20.0f,
					.z=j*distanceBetweenCubes};

			_lite_gl_gameObject_update(&TESTgameObject, instance);
		}
	}
}

static void _lite_gl_update(lite_engine_instance_t* instance){
	/*
	TODO : lock cursor to window and hide mouse
	SDL_WarpMouseInWindow(
	instance->SDLwindow, 
	instance->screenWidth,instance->screenHeight);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	*/
	/*TODO - Mefi - custom cursors*/

	/*
	 * to draw in wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
	*/

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	_lite_gl_handleSDLEvents(instance);
	_lite_gl_renderFrame(instance);
	SDL_GL_SwapWindow(instance->SDLwindow);

}

//TODO i hate passing the instance of the engine around everywhere.
//make it so you don't have to
void lite_gl_initialize(lite_engine_instance_t* instance){
	/*Set lite-engine function pointers*/
	instance->updateRenderer = &_lite_gl_update;

	/*set up SDL2*/
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
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (instance->SDLwindow == NULL){
		lite_printError("failed to create SDL_Window!",
				__FILE__, __LINE__);
	}

	instance->glContext = SDL_GL_CreateContext(instance->SDLwindow);
	if (instance->glContext == NULL){
		lite_printError("failed to create opengl context",
				__FILE__, __LINE__);
	}

	/*Set up glad*/
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)){
		lite_printError("failed to load GLAD",
				__FILE__,__LINE__);
	}

	/*set up opengl*/
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glViewport(0,0,instance->screenWidth, instance->screenHeight);

	printf("==========================================================\n");
	printf("OPENGL INFO\n");
	printf("Vendor\t%s\n", glGetString(GL_VENDOR));
	printf("Renderer\t%s\n", glGetString(GL_RENDERER));
	printf("Version\t%s\n", glGetString(GL_VERSION));
	printf("Shading Language\t%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("==========================================================\n");
}
