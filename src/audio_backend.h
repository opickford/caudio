#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

#include <stdint.h>

typedef struct AudioBackend AudioBackend;

typedef struct AudioBackendInfo
{
    uint32_t sample_rate;
    uint32_t channels;
    uint32_t buffer_frames;

} AudioBackendInfo;

AudioBackend* audio_backend_create(AudioBackendInfo* info);

void audio_backend_destroy(AudioBackend* backend);

uint32_t audio_backend_available_frames(AudioBackend* backend);

void audio_backend_write(AudioBackend* backend, const float* samples, uint32_t frames);

#endif