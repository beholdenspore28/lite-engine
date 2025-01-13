#ifndef LITE_ENGINE_H
#define LITE_ENGINE_H

#include "blib/blib.h"
#include "blib/blib_math3d.h"

#define BLIB_LOG_STRING "LITE_ENGINE"
#include "blib/blib_log.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
	int		is_running;
	double		time_current;
	long long	frame_current;
	double		time_delta;
	double		time_last;
	double		time_FPS;
} lite_engine_context_t;

void			lite_engine_start			(void);
int			lite_engine_is_running			(void);
void			lite_engine_update			(void);
void			lite_engine_stop			(void);
lite_engine_context_t	lite_engine_get_context			(void);
double			lite_engine_get_time_delta		(void);
void			lite_engine_ECS_update			(void);
long			lite_engine_entity_create		(void);
void			lite_engine_component_add		(uint64_t ID, uint64_t component_flag);
int			lite_engine_entity_has_component	(uint64_t entity_ID, uint64_t component_flag);

#endif
