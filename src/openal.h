#ifndef LAL_H
#define LAL_H

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

#include "math3d.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

typedef struct {
  ALuint *id;
  ALuint *buffer;
  unsigned int count;
} lal_audio_source;

lal_audio_source lal_audio_source_alloc(unsigned int count);
void lal_audio_source_free(lal_audio_source source);

void lal_audio_source_update(lal_audio_source source, vector3 position,
                             vector4 rotation);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

#endif
