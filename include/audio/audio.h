#ifndef AUDIO_H
#define AUDIO_H

#define COBJMACROS
#include <Audioclient.h>

#include <stdint.h>

#include "audio/wav.h"

typedef struct IMMDevice IMMDevice;

// TODO: Organise all this later.
typedef struct 
{
    float* data;

    uint32_t num_frames;
    uint32_t num_channels;

    uint32_t cursor;
} Sound;

Sound sound_from_wav(Wav* wav);

#define MAX_SOUNDS 5

// TODO: Store vector of playing sounds.
typedef struct
{
    // TODO: temp fixed array
    Sound sounds[MAX_SOUNDS]; 
    uint32_t num_sounds;

    IMMDevice* device;

    IAudioClient* pAudioClient;
    IAudioRenderClient* pRenderClient;

    WAVEFORMATEX* pwfx;

} Audio;

uint8_t audio_init(Audio* audio);

void audio_play(Audio* audio, Sound sound);

void audio_tick(Audio* audio);

void audio_destroy(Audio* audio);

// TODO: TEMP
void PrintDeviceName(IMMDevice* device);

#endif