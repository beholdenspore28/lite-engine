#ifndef LITE_ENGINE_H
#define LITE_ENGINE_H

#include "lite_engine_debug.h"
#include <stdint.h>
#include <stdlib.h>

enum {
	LITE_ENGINE_RENDERER_NONE,
	LITE_ENGINE_RENDERER_GL,
};

typedef uint8_t  ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

typedef struct {
	ui8 renderer;
	ui8 is_running;
} lite_engine_context_t;

void lite_engine_start(void);
lite_engine_context_t lite_engine_context(void);
ui8 lite_engine_is_running(void);
void lite_engine_update(void);
void lite_engine_stop(void);

#endif
