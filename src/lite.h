#ifndef LITE_H
#define LITE_H

#include <SDL2/SDL.h> //this should be removed from this file
#include <stdbool.h>
#include "HandmadeMath.h"

static const HMM_Vec3 lite_vec3_up =				(HMM_Vec3){ 0.0f, 1.0f, 0.0f};
static const HMM_Vec3 lite_vec3_down =	 		(HMM_Vec3){ 0.0f,-1.0f, 0.0f};
static const HMM_Vec3 lite_vec3_left =			(HMM_Vec3){-1.0f, 0.0f, 0.0f};
static const HMM_Vec3 lite_vec3_right =		(HMM_Vec3){ 1.0f, 0.0f, 0.0f};
static const HMM_Vec3 lite_vec3_forward =	(HMM_Vec3){ 0.0f, 0.0f, 1.0f};
static const HMM_Vec3 lite_vec3_back =			(HMM_Vec3){ 0.0f, 0.0f,-1.0f};
static const HMM_Vec3 lite_vec3_one =			(HMM_Vec3){ 1.0f, 1.0f, 1.0f};
static const HMM_Vec3 lite_vec3_zero =			(HMM_Vec3){ 0.0f, 0.0f, 0.0f};

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

	//time
	uint32_t frameStart;
	uint32_t frameEnd;
	float deltaTime;

	void (*updateRenderer) (struct lite_engine_instance_t*);
};

lite_engine_instance_t lite_engine_instance_create(
		lite_render_api renderApi, char* windowTitle, 
		int screenWidth, int screenHeight);

void lite_printError(
		const char* message, const char* file, unsigned int line);

float lite_time_inSeconds(lite_engine_instance_t* instance);
#endif//LITE_H
