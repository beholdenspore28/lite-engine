#ifndef LITE_H
#define LITE_H

#include <SDL2/SDL.h> //this should be removed from this file
#include <stdbool.h>

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
	char* windowTitle;
	SDL_GLContext glContext;
	bool engineRunning;

	void (*updateRenderer) (struct lite_engine_instance_t*);
};

void lite_printError(
		const char* message, const char* file, unsigned int line);

#endif//LITE_H
