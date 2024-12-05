#pragma once

#include "gl.h"

typedef struct {
	GLFWwindow *window;
	int         window_size_x;
	int         window_size_y;
	int         window_position_x;
	int         window_position_y;
	char       *window_title;
	int         window_fullscreen;
	int         window_always_on_top;
	float       time_current;
	float       time_last;
	float       time_delta;
	float       time_FPS;
	uint64_t    frame_current;
	vector3_t   ambient_light;
	camera_t    active_camera;
} lite_engine_context_t;

void lite_engine_start(void);
void lite_engine_update(void);

void lite_engine_set_context(lite_engine_context_t* context);
lite_engine_context_t* lite_engine_get_context(void);

void lite_engine_window_set_resolution(const int x, const int y);
void lite_engine_window_set_position(const int x, const int y);

void lite_engine_set_clear_color(const float r, const float g, const float b, const float a);

void primitive_draw_cube(transform_t transform, bool wireframe, vector4_t color);
