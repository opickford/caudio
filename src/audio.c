#include "audio.h"

#include "audio_backend.h"

#include "sfc/wav.h"

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>

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

typedef struct Audio
{
    AudioBackend* backend;
    AudioMixer mixer;
} Audio;

Sound sound_from_wav(Wav* wav)
{
    if (wav->fmt.NumChannels > 2)
    {
        printf("%u input channels not supported.", wav->fmt.NumChannels);
        return (Sound) { 0 };
    }

    // TODO: this is the number of individual channel samples, not frames.
    const uint32_t bytes_per_sample = wav->fmt.BitsPerSample / 8;
    const uint32_t num_samples = wav->dataSize / bytes_per_sample;

    float* data = malloc(num_samples * sizeof(float));
    if (!data)
    {
        printf("TODO: failed to malloc\n");
        return;
    }

    // TODO: handle BitsPerSample? how do we deal if the wav is not 16 bits per sample? (or if it is not 2 channels?)
    if (wav->fmt.BitsPerSample != 16)
    {
        printf("TODO: wav is not 16 bits per sample\n");
        return;
    }

    for (int i = 0; i < num_samples; ++i)
    {
        int16_t d = ((int16_t*)wav->data)[i];
        float f = d / (float)INT16_MAX;

        data[i] = f;
    }

    uint32_t bytes_per_frame = bytes_per_sample * wav->fmt.NumChannels;
    uint32_t num_frames = wav->dataSize / bytes_per_frame;

    Sound sound = {
        .data = data,
        .num_frames = num_frames,
        .num_channels = wav->fmt.NumChannels
    };

    return sound;
}

Audio* audio_create()
{
    Audio* audio = calloc(1, sizeof(Audio));

    if (!audio)
    {
        return 0; // TODO: handle failure.
    }

    AudioBackendInfo info;

    audio->backend = audio_backend_create(&info);
    if (!audio->backend)
    {
        free(audio);
        return 0;
    }

    // TODO: extract to audio_mixer_init?
    audio->mixer.output_channels = info.channels;
    audio->mixer.mix_buffer_frames = info.buffer_frames; // TODO: this doesn't really change unless reinit?

    audio->mixer.mix_buffer = malloc((size_t)info.buffer_frames * info.channels * sizeof(float));
    if (!audio->mixer.mix_buffer)
    {
        printf("Failed to allocate mix_buffer.\n");
        return 0;
    }

    return audio;
}

SoundInstance* audio_play(Audio* audio, Sound* sound)
{
    SoundInstance* inst = 0;
    for (int i = 0; i < audio->mixer.num_instances; ++i)
    {
        SoundInstance* old = &audio->mixer.instances[i];
        if (!old->playing)
        {
            // Found a sound to reuse.
            inst = old;
            break;
        }
    }

    if (!inst)
    {
        // No space for new instances.
        // TODO: TEMP: remove this limit when I have a dynamic array.

        if (audio->mixer.num_instances >= MAX_SOUND_INSTANCES)
        {
            printf("Max sounds reached.\n");
            return 0;
        }

        inst = &audio->mixer.instances[audio->mixer.num_instances++];
    }

    // Reset instance state.
    memset(inst, 0, sizeof(SoundInstance));

    inst->playing = 1;
    inst->volume = 1;
    inst->sound = sound;

    return inst;
}

static void write_sound(SoundInstance* inst, float* out, uint32_t frames, uint32_t out_channels)
{
    for (int i = 0; i < frames; ++i)
    {
        // Loop audio if at end, should only do this if sound is looping.
        if (inst->cursor >= inst->sound->num_frames)
        {
            if (inst->looping)
            {
                inst->cursor = 0;
            }
            else
            {
                inst->playing = 0;
                return;
            }
        }

        // TODO: i don't like this.
        float* input = inst->sound->data + inst->cursor * inst->sound->num_channels;

        // TODO: can definitely move if outside of loop.
        if (inst->sound->num_channels == 1)
        {
            for (int ch = 0; ch < out_channels; ++ch)
            {
                out[i * out_channels + ch] += input[0] * inst->volume;
            }
        }
        else if (inst->sound->num_channels == 2)
        {
            // TODO: For now we're just duplicating the stereo pair for all
            //       output channels. In the future we will handle differently.
            for (int ch = 0; ch < out_channels; ++ch) 
            {
                out[i * out_channels + ch] += input[ch % 2] * inst->volume;
            }
        }

        ++inst->cursor;
    }
}

static void audio_mixer_mix(AudioMixer* mixer, uint32_t frames)
{
    // TODO: note here we're using frames which is <= initial capacity of 
    // mix_buffer but do we need to assert this? what if the 
    //       system was reinitialised?
    memset(mixer->mix_buffer, 0, (size_t)frames * mixer->output_channels * sizeof(float));

    for (int i = 0; i < mixer->num_instances; ++i)
    {
        write_sound(&mixer->instances[i], mixer->mix_buffer, frames, mixer->output_channels);
    }
}

void audio_tick(Audio* audio)
{
    uint32_t num_frames_available = audio_backend_available_frames(audio->backend);
    if (num_frames_available == 0) return;

    audio_mixer_mix(&audio->mixer, num_frames_available);

    audio_backend_write(audio->backend, audio->mixer.mix_buffer, num_frames_available);
}

void audio_destroy(Audio* audio)
{
    audio_backend_destroy(audio->backend);

    // TODO: clear sounds? mixer?
    
}