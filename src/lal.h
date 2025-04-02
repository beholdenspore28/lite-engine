#ifndef LAL_H
#define LAL_H

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

#include "lgl.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

typedef struct {
  ALuint *id;
  ALuint *buffer;
  unsigned int count;
} audio_source_t;

audio_source_t audio_source_alloc(unsigned int count);
void audio_source_free(audio_source_t source);

void audio_source_update(audio_source_t source, l_object_t object,
                             lgl_context_t *lgl_context);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif
