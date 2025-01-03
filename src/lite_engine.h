#ifndef LITE_ENGINE_H
#define LITE_ENGINE_H

#include "blib/blib.h"
#include "blib/blib_math3d.h"
#include "lite_engine_debug.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

enum {
	LITE_ENGINE_RENDERER_NONE,
	LITE_ENGINE_RENDERER_GL,
};

enum {
	LITE_ENGINE_ENTITIES_MAX = 10,
};

typedef uint8_t	 ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

DECLARE_LIST(ui8)
DECLARE_LIST(ui16)
DECLARE_LIST(ui32)
DECLARE_LIST(ui64)

typedef struct {
	ui8    renderer;
	ui8    is_running;
	double time_current;
	ui64   frame_current;
	double time_delta;
	double time_last;
	double time_FPS;
} lite_engine_context_t;

void		      lite_engine_start(void);
void		      lite_engine_use_render_api(ui8 api);
ui8		      lite_engine_is_running(void);
void		      lite_engine_update(void);
void		      lite_engine_stop(void);
lite_engine_context_t lite_engine_get_context(void);
double		      lite_engine_get_time_delta(void);
void		      lite_engine_ECS_update(void);
ui64		      lite_engine_entity_create(void);
void lite_engine_component_add(uint64_t ID, uint64_t component_flag);
ui8  lite_engine_entity_has_component(uint64_t entity_ID,
				      uint64_t component_flag);

#endif
