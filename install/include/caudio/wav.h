#ifndef WAV_H
#define WAV_H

#include <stdint.h>

typedef struct
{
    uint32_t chunk_id;
    uint32_t chunk_size;
    uint32_t format;
} RiffHeader;

typedef struct {
    uint32_t chunk_id;
    uint32_t Subchunk1Size;
} WavFmtHeader;

typedef struct {
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;

    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    
} WavFmt;

typedef struct {
    WavFmt fmt;
    uint8_t* data;
    uint32_t dataSize;

} Wav;

Wav ReadWav(const char* fileName);

#endif