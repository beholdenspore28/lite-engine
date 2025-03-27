#include "lal.h"

lal_audio_source_t lal_audio_source_alloc(unsigned int count){

  lal_audio_source_t source;
  source.buffer = calloc(sizeof(*source.buffer), count);
  source.id     = calloc(sizeof(*source.id),     count);

  for(unsigned int i = 0; i < count; i++) {
    alGenBuffers(count, &source.buffer[i]);
    source.buffer[i] = alutCreateBufferFromFile("res/audio/random (1).wav");

    alGenSources(count,    &source.id[i]);
    alSourcei(source.id[i], AL_BUFFER,  source.buffer[i]);
    alSourcei(source.id[i], AL_LOOPING, AL_TRUE);
    alSourcef(source.id[i], AL_GAIN, 1.0);
    alSourcePlay(source.id[i]);
  }

  return source;
}

void lal_audio_source_update(
    lgl_object_t       object,
    lal_audio_source_t source,
    lgl_context_t      *lgl_context) {

  alSource3f(source.id, AL_POSITION,
      object.position->x,
      object.position->y,
      object.position->z);

  alListener3f(AL_POSITION,
      lgl_context->camera.position.x,
      lgl_context->camera.position.y,
      lgl_context->camera.position.z);

  vector3_t camera_up = vector3_rotate(
      vector3_up(1.0),
      lgl_context->camera.rotation);

  float orientation[6] = {
    lgl_context->camera.position.x,
    lgl_context->camera.position.y,
    lgl_context->camera.position.z,
    camera_up.x,
    camera_up.y,
    camera_up.z,
  };

  alListenerfv(AL_ORIENTATION, orientation);
}
