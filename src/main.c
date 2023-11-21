#include <SDL2/SDL.h>
#include "glad/glad.h"
#include "lite_gl.h"
#include "lite.h"

lite_gl_cube_t TESTcube;
lite_gl_camera_t TESTcamera;

int main(int argc, char** argv) {
	printf("\n[LITE-ENGINE] Rev up those fryers!\n\n");

	lite_engine_instance_t instance = 
		lite_engine_instance_create(
				LITE_RENDER_API_OPENGL, "Game Window",640,480);
	
	TESTcamera = lite_gl_camera_create(60.0f);
	TESTcube = lite_gl_cube_create();

	while(instance.engineRunning){
		/*delta time start*/
		instance.frameStart = SDL_GetTicks();	

 		instance.updateRenderer(&instance);

		/*delta time end*/
		instance.frameEnd = SDL_GetTicks();
		instance.deltaTime = 
			(((float)instance.frameEnd) - ((float)instance.frameStart)) * 0.001;
		// printf("frameStart: %i frameEnd: %i deltatime: %f\n", 
		// instance.frameStart, instance.frameEnd, instance.deltaTime);
	}

	return EXIT_SUCCESS;
}
