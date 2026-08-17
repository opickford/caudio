#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

#include "sfc/wav.h"

typedef struct SFC_Audio SFC_Audio;

// Represents the raw audio for a sound in float format.
typedef struct 
{
    float* data;

    uint32_t num_frames;
    uint32_t num_channels;

    // TODO: sample rate

} SFC_Sound;

// TODO: should be opaque, expose api for it instead.
typedef struct
{
    SFC_Sound* sound;
    uint32_t cursor;

    float volume;
    uint8_t looping;
    uint8_t playing;

} SFC_SoundInstance;

SFC_Sound sfc_sound_from_wav(Wav* wav);

SFC_Audio* sfc_audio_create();
void sfc_audio_destroy(SFC_Audio* audio);

SFC_SoundInstance* sfc_audio_play(SFC_Audio* audio, SFC_Sound* sound);

void sfc_audio_tick(SFC_Audio* audio);

#endif