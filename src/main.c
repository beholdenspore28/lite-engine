#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include "glad/glad.h"
#include "HandmadeMath.h"

#include "blib_file.h"
#include "lite_gl.h"
#include "lite.h"

void lite_printError(
		const char* message, const char* file, unsigned int line){
	fprintf(stderr, "[LITE-ENGINE-ERROR] in file \"%s\" line# %i \t%s\n"
			, file, line, message);
	exit(1);
}

lite_engine_instance_t lite_engine_instance_construct(
		lite_render_api renderApi, const char* windowTitle, 
		int screenWidth, int screenHeight) {
	lite_engine_instance_t instance;
	instance.screenWidth = screenWidth;
	instance.screenHeight = screenHeight;
	instance.engineRunning = true;
	instance.renderApi = renderApi;
	instance.windowTitle = windowTitle;

	switch (instance.renderApi){
		case LITE_RENDER_API_OPENGL:
			lite_glInitialize(&instance);	
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

int main(int argc, char** argv) {
	printf("\n[LITE-ENGINE] Rev up those fryers!\n\n");

	lite_engine_instance_t instance = 
		lite_engine_instance_construct(
				LITE_RENDER_API_OPENGL, "Game Window",854,480);

	lite_mesh_setup(&TESTmesh);
	TESTshader = lite_glPipeline_create();

	while(instance.engineRunning){
		instance.updateRenderer(&instance);
	}

	return EXIT_SUCCESS;
}
