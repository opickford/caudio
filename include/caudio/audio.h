#ifndef AUDIO_H
#define AUDIO_H


#include <stdint.h>

#include "caudio/wav.h"

typedef struct Audio Audio;


// TODO: Organise all this later.

// Represents the raw audio for a sound in float format.
typedef struct 
{
    float* data;

    uint32_t num_frames;
    uint32_t num_channels;

    // TODO: sample rate

} Sound;

// TODO: should be opaque.
typedef struct
{
    Sound* sound;
    uint32_t cursor;

    float volume;
    uint8_t looping;
    uint8_t playing;

} SoundInstance;

Sound sound_from_wav(Wav* wav);





Audio* audio_create();
void audio_destroy(Audio* audio);

SoundInstance* audio_play(Audio* audio, Sound* sound);

void audio_tick(Audio* audio);

#endif