#include "lite_gl.h"

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
		TESTcube.transform.position = BLIB_VEC3F_ZERO;
		TESTcube.transform.eulerAngles = BLIB_VEC3F_ZERO;
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
	lite_gl_camera_update(&TESTcamera, instance);
	lite_gl_cube_update(&TESTcube, instance);

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

			TESTcube.transform.position = 
				(blib_vec3f_t){
					.x=i*distanceBetweenCubes,
					.y=blib_noise_perlin2d(
							i*0.25f, j*0.25f, 0.2f, 2) * 20.0f,
					.z=j*distanceBetweenCubes};

			lite_gl_cube_update(&TESTcube, instance);
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
