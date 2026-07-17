#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

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
    Sound sound[MAX_SOUNDS]; 

    IMMDevice* device;
} Audio;

uint8_t audio_init(Audio* audio);

void audio_play();

void audio_tick();

void audio_destroy(Audio* audio);

#endif