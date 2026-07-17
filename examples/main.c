#include <stdio.h>

#define COBJMACROS
#include <mmDeviceapi.h>
#include <Windows.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <Audioclient.h>

#include <stdint.h>

#include "audio/audio.h"

typedef struct
{
    uint32_t chunk_id;
    uint32_t chunk_size;
    uint32_t format;

} riff_header;

typedef struct {
    uint32_t chunk_id;
    uint32_t Subchunk1Size;
} wave_fmt_header;

typedef struct {
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;

    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    
} wave_fmt;

typedef struct {
    wave_fmt fmt;
    BYTE* data;
    uint32_t dataSize;

} wave;


wave ReadWAV(const char* fileName)
{

    // http://soundfile.sapp.org/doc/WaveFormat/

    FILE* f = fopen(fileName, "rb");

    // 

    //WavHeader wh = { 0 };

    // TODO: Validate ids 

    riff_header rf = { 0 };
    fread(&rf, sizeof(riff_header), 1, f);

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

    wave wave = { 0 };

    wave_fmt fmt = { 0 };

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

void PrintDeviceName(IMMDevice* device)
{
    IPropertyStore* pProps = NULL;
    LPWSTR pwszID = NULL;

    HRESULT hr = IMMDevice_GetId(
        device,
        &pwszID
    );

    if (FAILED(hr))
    {
        printf("failed to get id\n");
        return -1;
    }

    hr = IMMDevice_OpenPropertyStore(device, STGM_READ, &pProps);

    if (FAILED(hr))
    {
        printf("failed to open property store\n");
        return -1;
    }

    PROPVARIANT varName = { 0 };
    hr = IPropertyStore_GetValue(pProps, &PKEY_Device_FriendlyName, &varName);

    if (FAILED(hr))
    {
        printf("failed to IPropertyStore_GetValue\n");
        return -1;
    }

    if (varName.vt != VT_EMPTY)
    {
        printf("Endpoint %S\n", varName.pwszVal);
    }

    CoTaskMemFree(pwszID);
    pwszID = NULL;
    PropVariantClear(&varName);

    IPropertyStore_Release(pProps);
}

void Play(IMMDevice* device)
{
    // TODO: CLeanup
    wave wave0 = ReadWAV("C:/Users/olive/source/repos/audio/examples/res/plankton.wav");
    wave wave1 = ReadWAV("C:/Users/olive/source/repos/audio/examples/res/omg.wav");


#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

    HRESULT hr;
    IAudioClient* pAudioClient = NULL;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;

    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE* pData;
    DWORD flags = 0;

    IAudioRenderClient* pRenderClient = NULL;


    hr = IMMDevice_Activate(
        device,
        &IID_IAudioClient,
        CLSCTX_ALL,
        NULL,
        (void**)&pAudioClient
    );

    if (FAILED(hr))
    {
        printf("Failed to IMMDevice_Activate\n");
        return;
    }

    WAVEFORMATEX* pwfx = NULL;
    hr = IAudioClient_GetMixFormat(pAudioClient, &pwfx);

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetMixFormat\n");
        return;
    }

    // TODO: Could try setting format to the closest match to the wav file. Although may not be 
    //       that difficult to manually convert.
    hr = IAudioClient_Initialize(
        pAudioClient,
        AUDCLNT_SHAREMODE_SHARED,
        0,
        hnsRequestedDuration,
        0,
        pwfx,
        NULL
    );

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_Initialize\n");
        return;
    }

    hr = IAudioClient_GetBufferSize(pAudioClient, &bufferFrameCount);

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetBufferSize\n");
        return;
    }

    printf("bufferFrameCount: %d\n", bufferFrameCount);

    hr = IAudioClient_GetService(
        pAudioClient,
        &IID_IAudioRenderClient,
        &pRenderClient
    );

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetService\n");
        return;
    }
    
    hr = IAudioRenderClient_GetBuffer(pRenderClient, bufferFrameCount, &pData);

    if (FAILED(hr))
    {
        printf("Failed to IAudioRenderClient_GetBuffer\n");
        return;
    }

    // TODO: TEMP: Fill buffer??
    // TODO: Definitely not correct.
    for (int i = 0; i < bufferFrameCount * pwfx->nBlockAlign; ++i)
    {
        pData[i] = MAXUINT8;
    }

    hr = IAudioRenderClient_ReleaseBuffer(pRenderClient, bufferFrameCount, flags);

    /*
    hr = IAudioRenderClient_GetBuffer(pRenderClient, bufferFrameCount, &pData);

    if (FAILED(hr))
    {
        printf("Failed to IAudioRenderClient_GetBuffer\n");
        return;
    }

    memcpy(pData, data, bufferFrameCount * fmt.BlockAlign);
    hr = IAudioRenderClient_ReleaseBuffer(pRenderClient, bufferFrameCount, flags);

    if (FAILED(hr))
    {
        printf("failed to releaase buffer\n");
    }*/


    // TODO: Consider samplerates.

    if (wave0.fmt.NumChannels != pwfx->nChannels || wave0.fmt.BitsPerSample != pwfx->wBitsPerSample)
    {
        printf("mismatch formats.\n");
    }

    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

    hr = IAudioClient_Start(pAudioClient);
    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_Start\n");
        return;
    }

    UINT32 framesWritten = 0;
    UINT32 totalFrames = wave0.dataSize / wave0.fmt.BlockAlign;
    
    while (framesWritten < totalFrames)
    {
        // This gets frames that are going to be played.
        hr = IAudioClient_GetCurrentPadding(pAudioClient, &numFramesPadding);
        if (FAILED(hr))
        {
            printf("Failed to IAudioClient_GetCurrentPadding\n");
            return;
        }

        // Calculate how much space there is for more frames.
        numFramesAvailable = bufferFrameCount - numFramesPadding;

        if (numFramesAvailable == 0)
        {
            Sleep(1);
            continue;
        }
        if (numFramesAvailable > totalFrames - framesWritten)
        {
            numFramesAvailable = totalFrames - framesWritten;
        }


        //printf("numFramesAvailable: %d\n", numFramesAvailable);
        //printf("numFramesPadding: %d\n", numFramesPadding);

        hr = IAudioRenderClient_GetBuffer(pRenderClient, numFramesAvailable, &pData);

        if (FAILED(hr))
        {
            printf("Failed to IAudioRenderClient_GetBuffer\n");
            return;
        }

        float frequency = 450.0f; // A4
        float amplitude = 0.25f;
        static double theta = 0.0;
        double thetaIncrement = 2.0 * 3.14159 * frequency / pwfx->nSamplesPerSec;

        float* pFloatData = (float*)pData;
        float* pFloatDataStart = pFloatData;
        UINT32 frames = numFramesAvailable;
        UINT32 channels = pwfx->nChannels;

        // Input data is 16bits per sample 
        // we need to convert to 32 bits eexpected output.

        // also input data is 2 channel, output is 8/
        
        int16_t* input = wave0.data; 

        for (UINT32 i = 0; i < frames; ++i)
        {            
            // 2 channels in input
            uint32_t frameI = (framesWritten + i) * 2;

            int16_t d0 = input[frameI];
            float f0 = d0 / (float)INT16_MAX;

            int16_t d1 = input[frameI + 1];
            float f1 = d1 / (float)INT16_MAX;

            for (UINT32 ch = 0; ch < channels; ++ch)
            {
                if (ch % 2 == 0)
                {
                    *pFloatData++ = f0;
                }
                else
                {
                    *pFloatData++ = f1;
                }
                
            }

        }

        pFloatData = pFloatDataStart;

        int16_t* input1 = wave1.data;

        for (UINT32 i = 0; i < frames; ++i)
        {
            // 2 channels in input
            uint32_t frameI = (framesWritten + i) * 2;

            // Handle end of audio.
            if (frameI > wave1.dataSize / 2) break;

            int16_t d0 = input1[frameI];
            float f0 = d0 / (float)INT16_MAX;

            int16_t d1 = input1[frameI + 1];
            float f1 = d1 / (float)INT16_MAX;

            for (UINT32 ch = 0; ch < channels; ++ch)
            {
                if (ch % 2 == 0)
                {
                    *pFloatData++ += f0;
                }
                else
                {
                    *pFloatData++ += f1;
                }

            }

        }

        //memcpy(pData, data + framesWritten * fmt.BlockAlign, numFramesAvailable * fmt.BlockAlign);
        
        hr = IAudioRenderClient_ReleaseBuffer(pRenderClient, numFramesAvailable, flags);

        if (FAILED(hr))
        {
            printf("Failed to IAudioRenderClient_ReleaseBuffer\n");
            return;
        }

        framesWritten += numFramesAvailable;

    }

    printf("done\n");

}

int main()
{
    Audio audio;
    if (!audio_init(&audio))
    {
        return -1;
    }

    PrintDeviceName(audio.device);

    Play(audio.device);

    audio_destroy(&audio);
    
	return 0;
}
