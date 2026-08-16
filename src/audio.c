#include "audio/audio.h"

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

#define COBJMACROS
#include <mmDeviceapi.h>
#include <Windows.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <Audioclient.h>

#include "audio/wav.h"

// TODO: Define in separate header?
const CLSID CLSID_MMDeviceEnumerator = { 0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E } };
const IID IID_IMMDeviceEnumerator = { 0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 } };
const IID IID_IAudioMeterInformation = { 0xC02216F6, 0x8C67, 0x4B5B, { 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64 } };

const IID IID_IAudioClient = { 0x1CB9AD4C, 0xDBFA, 0x4c32, {0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2} };
const IID IID_IAudioRenderClient = { 0xF294ACFC, 0x3146, 0x4483, {0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2} };

// TODO: some device header?
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

// TODO: Some internal device file??
void GetDefaultDevice(IMMDevice** device)
{
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDeviceCollection* pCollection = NULL;
    IMMDevice* pEndpoint = NULL;

    HRESULT hr = CoCreateInstance(
        &CLSID_MMDeviceEnumerator,
        NULL,
        CLSCTX_ALL,
        &IID_IMMDeviceEnumerator,
        (void**)(&pEnumerator)
    );

    if (FAILED(hr))
    {
        printf("failed to create enumerator\n");
        return -1;
    }

    if (!pEnumerator)
    {
        printf("pEnumerator is null.\n");
        return -1;
    }

    IMMDeviceEnumerator_EnumAudioEndpoints(
        pEnumerator,
        eRender,
        DEVICE_STATE_ACTIVE,
        &pCollection
    );

    if (!pCollection)
    {
        printf("pCollection is null.\n");
        return -1;
    }

    IMMDeviceEnumerator_GetDefaultAudioEndpoint(pEnumerator, eRender, eConsole, &pEndpoint);

    if (FAILED(hr))
    {
        printf("failed to get endpoint\n");
        return -1;
    }


    IMMDeviceEnumerator_Release(pEnumerator);
    IMMDeviceCollection_Release(pCollection);

    *device = pEndpoint;
}

Sound sound_from_wav(Wav* wav)
{
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
        .num_channels = wav->fmt.NumChannels,
        .cursor = 0
    };

    return sound;
}

// TODO: How do we return success/failure? (a common issue of mine.)
uint8_t audio_init(Audio* audio)
{
    memset(audio, 0, sizeof(Audio));

    // Initialise COM.
    HRESULT hr = CoInitialize(NULL);

    if (FAILED(hr))
    {
        printf("failed to init COM\n");
        return 0;
    }

    GetDefaultDevice(&audio->device);
    
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

    // TODO: this is only requesting a second, is this correct?
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;

    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE* pData;
    DWORD flags = 0;

    hr = IMMDevice_Activate(
        audio->device,
        &IID_IAudioClient,
        CLSCTX_ALL,
        NULL,
        (void**)&audio->pAudioClient
    );

    if (FAILED(hr))
    {
        printf("Failed to IMMDevice_Activate\n");
        return;
    }

    
    hr = IAudioClient_GetMixFormat(audio->pAudioClient, &audio->pwfx);
    

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetMixFormat\n");
        return;
    }

    // TODO: better way of logging
    printf("Num Channels: %u\n", audio->pwfx->nChannels);
    printf("Bits Per Sample: %u\n", audio->pwfx->wBitsPerSample);
    printf("Block Align: %u\n", audio->pwfx->nBlockAlign);
    printf("Format Tag: %u\n", audio->pwfx->wFormatTag);
    printf("Samples Per Second: %u\n", audio->pwfx->nSamplesPerSec);
    
    // TODO: Could try setting format to the closest match to the wav file. Although may not be 
    //       that difficult to manually convert.
    hr = IAudioClient_Initialize(
        audio->pAudioClient,
        AUDCLNT_SHAREMODE_SHARED,
        0,
        hnsRequestedDuration,
        0,
        audio->pwfx,
        NULL
    );

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_Initialize\n");
        return;
    }

    hr = IAudioClient_GetBufferSize(audio->pAudioClient, &bufferFrameCount);

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetBufferSize\n");
        return;
    }

    printf("bufferFrameCount: %d\n", bufferFrameCount);

    hr = IAudioClient_GetService(
        audio->pAudioClient,
        &IID_IAudioRenderClient,
        &audio->pRenderClient
    );

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetService\n");
        return;
    }

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


    // // TODO: Consider samplerates.

    // if (wave0.fmt.NumChannels != pwfx->nChannels || wave0.fmt.BitsPerSample != pwfx->wBitsPerSample)
    // {
    //     printf("mismatch formats.\n");
    // }

    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / audio->pwfx->nSamplesPerSec;

    hr = IAudioClient_Start(audio->pAudioClient);
    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_Start\n");
        return;
    }

    return 1;
}

void audio_play(Audio* audio, Sound sound)
{
    // TODO: TEMP: remove this limit when I have a dynamic array.
    if (audio->num_sounds >= MAX_SOUNDS)
    {
        printf("Max sounds reached.\n");
        return;
    }

    audio->sounds[audio->num_sounds++] = sound;
}

void write_sound(Audio* audio, Sound* sound, float* out, uint32_t frames)
{
    // TODO: stop hardcoding channels, need to get this from the sound data.
    uint32_t in_channels = 2;
    uint32_t out_channels = 2;
   
    for (UINT32 i = 0; i < frames; ++i)
    {
        // Loop audio if at end, should only do this if sound is looping.
        if (sound->cursor >= sound->num_frames)
        {
            sound->cursor = 0;
        }

        // TODO: i don't like this.
        float* input = sound->data + sound->cursor * in_channels;

        // TODO: this filling loop must be fast.
        for (UINT32 ch = 0; ch < out_channels; ++ch)
        {
            // TODO: hack for just duplicating the 2 channels, better way would be much nicer.
            // TODO: doesn't even work 
            if (ch % 2 == 0)
            {
                out[i * out_channels + ch] += input[ch];
            }
            else
            {
                out[i * out_channels + ch] += input[ch];
            }
        }

        ++sound->cursor;
    }
}

void audio_tick(Audio* audio)
{
    HRESULT hr;

    UINT32 bufferFrameCount;
    hr = IAudioClient_GetBufferSize(audio->pAudioClient, &bufferFrameCount);

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetBufferSize\n");
        return;
    }

    printf("bufferFrameCount: %d\n", bufferFrameCount);

    DWORD flags = 0;
    //UINT32 totalFrames = wave0.dataSize / wave0.fmt.BlockAlign;
    
    // TODO: do we want to write all the frames in one tick? Or do we want to write a few frames each tick? (probably the latter)
    //while (framesWritten < totalFrames)
    //{
        // This gets frames that are going to be played.
    UINT32 numFramesPadding;
    hr = IAudioClient_GetCurrentPadding(audio->pAudioClient, &numFramesPadding);
    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetCurrentPadding\n");
        return;
    }
    
    // Determine if we can write.
    UINT32 numFramesAvailable = bufferFrameCount - numFramesPadding;
    if (numFramesAvailable == 0)
    {
        return;
    }

    printf("numFramesAvailable: %d\n", numFramesAvailable);

    BYTE* pData;
    hr = IAudioRenderClient_GetBuffer(audio->pRenderClient, numFramesAvailable, &pData);

    if (FAILED(hr))
    {
        printf("Failed to IAudioRenderClient_GetBuffer\n");
        return;
    }

    float* pFloatData = (float*)pData;
    float* pFloatDataStart = pFloatData;
    UINT32 frames = numFramesAvailable;
    //UINT32 channels = pwfx->nChannels;

    // Clear sound buffer.
    memset(pData, 0, numFramesAvailable * (size_t)audio->pwfx->nBlockAlign);


    // Input data is 16bits per sample 
    // we need to convert to 32 bits expected output.

    // also input data is 2 channel, output is 8/
       
    // TODO: go through each sound, check if it has finished playing, if not, write to the output buffer.

    // TODO: what happens to the audio->pwfx->nBlockAlign?
    for (int i = 0; i < audio->num_sounds; ++i)
    {
        write_sound(audio, &audio->sounds[i], pFloatData, frames);
    }
        
    hr = IAudioRenderClient_ReleaseBuffer(audio->pRenderClient, numFramesAvailable, flags);

    if (FAILED(hr))
    {
        printf("Failed to IAudioRenderClient_ReleaseBuffer\n");
        return;
    }
}

void audio_destroy(Audio* audio)
{
    // TODO: clear sounds?
    if (audio->device) IMMDevice_Release(audio->device);
    audio->device = NULL;

    CoUninitialize();
}