#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

typedef struct SFC_Audio SFC_Audio;
typedef struct SFC_Sound SFC_Sound;
typedef struct SFC_SoundInstance SFC_SoundInstance;

SFC_Audio* sfc_audio_create();
void sfc_audio_destroy(SFC_Audio* audio);

SFC_SoundInstance* sfc_audio_play(SFC_Audio* audio, SFC_Sound* sound);

void sfc_audio_tick(SFC_Audio* audio);

#endif