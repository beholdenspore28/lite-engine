#include <stdio.h>
#include <SDL2/SDL.h>
#include "glad/glad.h"
#include "lite_gl.h"
#include "lite.h"

lite_gl_gameObject_t TESTgameObject;
lite_gl_gameObject_t TESTgameObject2;
lite_gl_camera_t TESTcamera;

int main(int argc, char** argv) {
	printf("\n[LITE-ENGINE] Rev up those fryers!\n\n");

	lite_engine_instance_t instance = 
		lite_engine_instance_create(
				LITE_RENDER_API_OPENGL, "Game Window",854,480);

	TESTcamera = lite_gl_camera_create(&instance, 60.0f);
	TESTgameObject = lite_gl_gameObject_create();
	TESTgameObject2 = lite_gl_gameObject_create();
	TESTgameObject2.transform.position = blib_vec3f_scale(BLIB_VEC3F_RIGHT,5.0f);

	while(instance.engineRunning){
 		instance.updateRenderer(&instance);
	}

	return EXIT_SUCCESS;
}
