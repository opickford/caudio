#include "wav.h"

#include "sound.h"
#include "sound_internal.h"

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

Wav sfc_wav_read(const char* fileName)
{
    // http://soundfile.sapp.org/doc/WaveFormat/
    FILE* f = fopen(fileName, "rb");

    // TODO: Validate ids 

    RiffHeader rf = { 0 };
    fread(&rf, sizeof(RiffHeader), 1, f);

    if (rf.chunk_id != 0x46464952)
    {
        printf("invalid rf chunk id\n");
        return (Wav) { 0 };
    }
    if (rf.format != 0x45564157)
    {
        printf("invalid rf format\n");
        return (Wav) { 0 };
    }

    // Search for next chunks.
    uint32_t chunkId = 0;
    uint32_t chunkSize = 0;

    Wav wave = { 0 };
    WavFmt fmt = { 0 };

    while (fread(&chunkId, sizeof(chunkId), 1, f)) 
    {
        if (fread(&chunkSize, sizeof(chunkSize), 1, f) != 1)
        {
            // Found malformed chunk.
            // TODO: do something? just skipping it here.
            break;
        }

        if (chunkId == 0x20746d66)
        {
            fread(&fmt, sizeof(fmt), 1, f);
        }
        else if (chunkId == 0x61746164)
        {
            wave.dataSize = chunkSize;
            wave.data = malloc(wave.dataSize);

            if (!wave.data)
            {
                printf("Failed to malloc for WAV data.\n");
                return (Wav) { 0 };
            }
            
            size_t bytes_read = fread(wave.data, 1, wave.dataSize, f);

            // Ensure correct number of bytes were read.
            if (bytes_read != wave.dataSize)
            {
                free(wave.data);
                wave.data = 0;
                wave.dataSize = 0;
                printf("Unexpected EOF when reading WAV.\n");
                fclose(f);
                return (Wav) { 0 };
            }
        }
        else
        {
            // unknown chunkId, skip size of chunk. RIFF format supports different chunks basically.
            fseek(f, chunkSize, SEEK_CUR);
        }
    }

    wave.fmt = fmt;

   


    /*
    if (header.AudioFormat != 1)
    {
        printf("ERROR: .WAV NOT PCM! AudioFormat: %d\n", header.AudioFormat);
        return;
    }*/

    /*
    data = malloc(header.SubChunk2Size);
    if (!data)
    {
        header = (WavHeader){ 0 };
        return;
    }

    fread(data, header.SubChunk2Size, 1, f);

    for (int i = 0; i < header.SubChunk2Size; ++i)
    {
        printf("%f ", data[i]);
    }
    */
    fclose(f);

    return wave;
}

SFC_Sound* sfc_sound_from_wav(const Wav* wav)
{
    if (wav->fmt.NumChannels > 2)
    {
        printf("%u input channels not supported.", wav->fmt.NumChannels);
        return 0;
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

    SFC_Sound* sound = calloc(1, sizeof(SFC_Sound));
    if (!sound) return 0;

    sound->data = data;
    sound->num_frames = num_frames;
    sound->num_channels = wav->fmt.NumChannels;

    return sound;
}