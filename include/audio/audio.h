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
    uint32_t cursor;
    uint32_t size;

    float* data;
} Sound;

#define MAX_SOUNDS 5

// TODO: Store vector of playing sounds.
typedef struct
{
    // TODO: temp fixed array
    Sound sounds[MAX_SOUNDS]; 

    IMMDevice* device;

    IAudioClient* pAudioClient;
    IAudioRenderClient* pRenderClient;

    WAVEFORMATEX* pwfx;

    uint32_t num_sounds;
} Audio;

uint8_t audio_init(Audio* audio);

void audio_play(Audio* audio, Wav wav);

void audio_tick(Audio* audio);

void audio_destroy(Audio* audio);

// TODO: TEMP
void PrintDeviceName(IMMDevice* device);

#endif