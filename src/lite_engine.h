#ifndef LITE_ENGINE_H
#define LITE_ENGINE_H

#include "lite_engine_debug.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

enum {
	LITE_ENGINE_RENDERER_NONE,
	LITE_ENGINE_RENDERER_GL,
};

enum {
	LITE_ENGINE_ECS_COMPONENT_TRANSFORM,
};

typedef uint8_t  ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

void   lite_engine_start                     (void);
void   lite_engine_use_render_api            (ui8 api);
ui8    lite_engine_is_running                (void);
void   lite_engine_update                    (void);
void   lite_engine_stop                      (void);

ui64   lite_engine_ECS_get_count             (void);
ui64   lite_engine_ECS_get_capacity          (void);
void   lite_engine_ECS_start                 (void);
void   lite_engine_ECS_stop                  (void);
ui64   lite_engine_ECS_entity_create         (void);
void   lite_engine_ECS_component_add         (ui64 entity, ui8 component);

#endif
