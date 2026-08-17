#include "sfc/wav.h"

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