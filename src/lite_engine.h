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
	void		*platform_data;
	int		is_running;
	double		time_current;
	long long	frame_current;
	double		time_delta;
	double		time_last;
	double		time_FPS;
} lite_engine_context_t;

lite_engine_context_t *	lite_engine_start	(void);
int			lite_engine_is_running	(void);
void			lite_engine_end_frame	(lite_engine_context_t *engine);
void			lite_engine_stop	(lite_engine_context_t *engine);

#endif
