#ifndef AUDIO_H
#define AUDIO_H

#define COBJMACROS
#include <Audioclient.h>

#include <stdint.h>

#include "audio/wav.h"

typedef struct IMMDevice IMMDevice;

// TODO: Organise all this later.

// Represents the raw audio for a sound in float format.
typedef struct 
{
    float* data;

    uint32_t num_frames;
    uint32_t num_channels;

} Sound;

typedef struct
{
    Sound* sound;
    uint32_t cursor;

    float volume;
    uint8_t looping;

    uint8_t playing;

} SoundInstance;

Sound sound_from_wav(Wav* wav);

#define MAX_SOUND_INSTANCES 100

typedef struct
{
    uint32_t output_channels;
    //uint32_t output_sample_rate;

    float* mix_buffer;
    uint32_t mix_buffer_frames;

    // TODO: temporary fixed buffer.
    SoundInstance instances[MAX_SOUND_INSTANCES];
    uint32_t num_instances;

} AudioMixer;


// TODO: Store vector of playing sounds.
typedef struct
{
    IMMDevice* device;

    IAudioClient* pAudioClient;
    IAudioRenderClient* pRenderClient;

    WAVEFORMATEX* pwfx; // TODO: rename this

    AudioMixer mixer;

} Audio;


uint8_t audio_init(Audio* audio);

SoundInstance* audio_play(Audio* audio, Sound* sound);

void audio_tick(Audio* audio);

void audio_mixer_mix(AudioMixer* mixer, uint32_t frames);

void audio_destroy(Audio* audio);

// TODO: TEMP
void PrintDeviceName(IMMDevice* device);

#endif