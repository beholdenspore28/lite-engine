#include "lite.h"
#include "lite_gl.h"

const HMM_Vec3 lite_vec3_up =		(HMM_Vec3){0.0f, 1.0f, 0.0f};
const HMM_Vec3 lite_vec3_down =	 	(HMM_Vec3){0.0f, 0.0f, 0.0f};
const HMM_Vec3 lite_vec3_left =		(HMM_Vec3){0.0f, 0.0f, 0.0f};
const HMM_Vec3 lite_vec3_right =	(HMM_Vec3){0.0f, 0.0f, 0.0f};
const HMM_Vec3 lite_vec3_forward =	(HMM_Vec3){0.0f, 0.0f, 0.0f};
const HMM_Vec3 lite_vec3_back =		(HMM_Vec3){0.0f, 0.0f, 0.0f};

void lite_printError(
		const char* message, const char* file, unsigned int line){
	fprintf(stderr, "[LITE-ENGINE-ERROR] in file \"%s\" line# %i \t%s\n"
			, file, line, message);
	exit(1);
}

lite_engine_instance_t lite_engine_instance_create(
		lite_render_api renderApi, char* windowTitle, 
		int screenWidth, int screenHeight) {
	lite_engine_instance_t instance;
	instance.screenWidth = screenWidth;
	instance.screenHeight = screenHeight;
	instance.engineRunning = true;
	instance.renderApi = renderApi;
	instance.windowTitle = windowTitle;

	switch (instance.renderApi){
		case LITE_RENDER_API_OPENGL:
			lite_gl_initialize(&instance);	
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

float lite_time_inSeconds(lite_engine_instance_t* instance) {
	return ((float)instance->frameStart) * 0.001f;
}
