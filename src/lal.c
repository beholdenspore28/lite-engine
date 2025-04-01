#include "lal.h"

lal_audio_source_t lal_audio_source_alloc(unsigned int count) {

  lal_audio_source_t source;
  source.buffer = calloc(sizeof(ALuint), count);
  source.id = calloc(sizeof(ALuint), count);
  source.count = count;

  for (unsigned int i = 0; i < count; i++) {
    alGenBuffers(count, &source.buffer[i]);
    source.buffer[i] = alutCreateBufferFromFile("res/audio/random (1).wav");

    alGenSources(count, &source.id[i]);
    alSourcei(source.id[i], AL_BUFFER, source.buffer[i]);
    alSourcei(source.id[i], AL_LOOPING, AL_TRUE);
    alSourcef(source.id[i], AL_GAIN, 1.0);
    alSourcePlay(source.id[i]);
  }

  return source;
}

void lal_audio_source_free(lal_audio_source_t source) {

  for (unsigned int i = 0; i < source.count; i++) {
    alDeleteBuffers(source.count, source.buffer + i);
    alDeleteSources(source.count, source.id + i);
  }

  free(source.buffer);
  free(source.id);
}

void lal_audio_source_update(lal_audio_source_t source, lgl_batch_t batch,
                             lgl_context_t *lgl_context) {

  alListener3f(AL_POSITION, lgl_context->camera.position.x,
               lgl_context->camera.position.y, lgl_context->camera.position.z);

  vector3_t listener_up = (vector3_t){0.0, 0.0, 1.0};
  vector3_t listener_at = (vector3_t){0.0, 1.0, 0.0};

  listener_at = vector3_rotate(listener_at, lgl_context->camera.rotation);
  listener_up = vector3_rotate(listener_up, lgl_context->camera.rotation);

  float orientation[6] = {
      listener_at.x, listener_at.y, listener_at.z,
      listener_up.x, listener_up.y, listener_up.z,
  };

  alListenerfv(AL_ORIENTATION, orientation);

  for (unsigned int i = 0; i < source.count; i++) {
    alSource3f(source.id[i], AL_POSITION, batch.position[i].x,
               batch.position[i].y, batch.position[i].z);
  }
}
