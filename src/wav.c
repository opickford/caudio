#include "caudio/wav.h"

#include <stdio.h>
#include <stdlib.h>

Wav ReadWav(const char* fileName)
{

    // http://soundfile.sapp.org/doc/WaveFormat/

    FILE* f = fopen(fileName, "rb");

    // 

    //WavHeader wh = { 0 };

    // TODO: Validate ids 

    RiffHeader rf = { 0 };
    fread(&rf, sizeof(RiffHeader), 1, f);

    if (rf.chunk_id != 0x46464952)
    {
        printf("invalid rf chunk id\n");
        return;
    }
    if (rf.format != 0x45564157)
    {
        printf("invalid rf format\n");
        return;
    }

    // Search for next chunks.
    uint32_t chunkId = 0;
    uint32_t chunkSize = 0;

    Wav wave = { 0 };

    WavFmt fmt = { 0 };

    while (fread(&chunkId, sizeof(chunkId), 1, f)) 
    {
        fread(&chunkSize, sizeof(chunkSize), 1, f);

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
                printf("failed to mallco\n");
                return;
            }
            
            fread(wave.data, 1, wave.dataSize, f);

            // TODO: Ensure correct number of elements read.
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