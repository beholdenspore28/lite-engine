#include <stdio.h>
#include <SDL2/SDL.h>
#include "glad/glad.h"
#include "lite_gl.h"
#include "lite.h"

lite_gl_gameObject_t TESTgameObject;
lite_gl_camera_t TESTcamera;

int main(int argc, char** argv) {
	printf("\n[LITE-ENGINE] Rev up those fryers!\n\n");

	lite_engine_instance_t instance = 
		lite_engine_instance_create(
				LITE_RENDER_API_OPENGL, "Game Window",854,600);

	TESTcamera = lite_gl_camera_create(&instance, 60.0f);
	TESTgameObject = lite_gl_gameObject_create();


	while(instance.engineRunning){
 		instance.updateRenderer(&instance);
	}

	return EXIT_SUCCESS;
}
