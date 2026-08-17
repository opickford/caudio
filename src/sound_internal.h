#ifndef SOUND_INTERNAL_H
#define SOUND_INTERNAL_H

#include <stdint.h>

#include "sfc/sound.h"

// Represents the raw audio for a sound in float format.
typedef struct SFC_Sound
{
    float* data;

    uint32_t num_frames;
    uint32_t num_channels;

    // TODO: sample rate

} SFC_Sound;

// TODO: should be opaque, expose api for it instead.
typedef struct SFC_SoundInstance
{
    SFC_Sound* sound;
    uint32_t cursor;

    float volume;
    uint8_t looping;
    uint8_t playing;

} SFC_SoundInstance;

#endif