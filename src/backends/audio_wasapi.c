#include "audio_backend.h"

#define COBJMACROS
#include <Audioclient.h>
#include <mmDeviceapi.h>
#include <Windows.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

typedef struct AudioBackend
{
    IMMDevice* device;

    IAudioClient* pAudioClient;
    IAudioRenderClient* pRenderClient;

    WAVEFORMATEX* pwfx; // TODO: rename this

} AudioBackend;

void PrintDeviceName(IMMDevice* device);

// TODO: Define in separate header?
const CLSID CLSID_MMDeviceEnumerator = { 0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E } };
const IID IID_IMMDeviceEnumerator = { 0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 } };
const IID IID_IAudioMeterInformation = { 0xC02216F6, 0x8C67, 0x4B5B, { 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64 } };

const IID IID_IAudioClient = { 0x1CB9AD4C, 0xDBFA, 0x4c32, {0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2} };
const IID IID_IAudioRenderClient = { 0xF294ACFC, 0x3146, 0x4483, {0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2} };

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

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

AudioBackend* audio_backend_create(AudioBackendInfo* info)
{
    // TODO: how do we use this config?

    AudioBackend* backend = calloc(1, sizeof(AudioBackend));
    if (!backend)
    {
        return 0;
    }

    // TODO: free backend on failures.

    // Initialise COM.
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        printf("failed to init COM\n");
        return 0;
    }

    GetDefaultDevice(&backend->device);

    // TODO: make configurable?
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_MILLISEC * 20; // 20ms, causes a delay otherwise.
    REFERENCE_TIME hnsActualDuration;

    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE* pData;
    DWORD flags = 0;

    hr = IMMDevice_Activate(
        backend->device,
        &IID_IAudioClient,
        CLSCTX_ALL,
        NULL,
        (void**)&backend->pAudioClient
    );

    if (FAILED(hr))
    {
        printf("Failed to IMMDevice_Activate\n");
        return 0;
    }


    hr = IAudioClient_GetMixFormat(backend->pAudioClient, &backend->pwfx);


    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetMixFormat\n");
        return 0;
    }

    // TODO: better way of logging
    printf("Num Channels: %u\n", backend->pwfx->nChannels);
    printf("Bits Per Sample: %u\n", backend->pwfx->wBitsPerSample);
    printf("Block Align: %u\n", backend->pwfx->nBlockAlign);
    printf("Format Tag: %u\n", backend->pwfx->wFormatTag);
    printf("Samples Per Second: %u\n", backend->pwfx->nSamplesPerSec);

    // TODO: Could try setting format to the closest match to the wav file. Although may not be 
    //       that difficult to manually convert.
    hr = IAudioClient_Initialize(
        backend->pAudioClient,
        AUDCLNT_SHAREMODE_SHARED,
        0,
        hnsRequestedDuration,
        0,
        backend->pwfx,
        NULL
    );

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_Initialize\n");
        return 0;
    }

    hr = IAudioClient_GetBufferSize(backend->pAudioClient, &bufferFrameCount);

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetBufferSize\n");
        return;
    }

    printf("bufferFrameCount: %d\n", bufferFrameCount);

    

    hr = IAudioClient_GetService(
        backend->pAudioClient,
        &IID_IAudioRenderClient,
        &backend->pRenderClient
    );

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetService\n");
        return 0;
    }


    // // TODO: Consider samplerates.

    // if (wave0.fmt.NumChannels != pwfx->nChannels || wave0.fmt.BitsPerSample != pwfx->wBitsPerSample)
    // {
    //     printf("mismatch formats.\n");
    // }

    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / backend->pwfx->nSamplesPerSec;

    printf("Buffer: %u frames (%.2f ms)\n", bufferFrameCount, 1000.0 * (double)bufferFrameCount /
        backend->pwfx->nSamplesPerSec);

    hr = IAudioClient_Start(backend->pAudioClient);
    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_Start\n");
        return;
    }

    info->channels = backend->pwfx->nChannels;
    info->buffer_frames = bufferFrameCount;
    info->sample_rate = backend->pwfx->nSamplesPerSec;

    PrintDeviceName(backend->device);

    return backend;
}

void audio_backend_destroy(AudioBackend* backend)
{
    // TODO:
    if (backend->device) IMMDevice_Release(backend->device);
    backend->device = NULL;

    CoUninitialize();
}

uint32_t audio_backend_available_frames(AudioBackend* backend)
{
    HRESULT hr;

    UINT32 bufferFrameCount;
    hr = IAudioClient_GetBufferSize(backend->pAudioClient, &bufferFrameCount);

    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetBufferSize\n");
        return;
    }

    DWORD flags = 0;

    // TODO: should log the padding to see how much delay we might have.
    UINT32 numFramesPadding;
    hr = IAudioClient_GetCurrentPadding(backend->pAudioClient, &numFramesPadding);
    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_GetCurrentPadding\n");
        return;
    }

    // Determine if we can write.
    UINT32 numFramesAvailable = bufferFrameCount - numFramesPadding;
    return numFramesAvailable;
}

void audio_backend_write(AudioBackend* backend, const float* samples, uint32_t frames)
{
    BYTE* pData;
    HRESULT hr = IAudioRenderClient_GetBuffer(backend->pRenderClient, frames, &pData);

    if (FAILED(hr))
    {
        printf("Failed to IAudioRenderClient_GetBuffer\n");
        return;
    }

    // TODO: this will have to change if we end up not writing float data..
    memcpy((float*)pData, samples, (size_t)frames * backend->pwfx->nBlockAlign);

    DWORD flags = 0;
    hr = IAudioRenderClient_ReleaseBuffer(backend->pRenderClient, frames, flags);

    if (FAILED(hr))
    {
        printf("Failed to IAudioRenderClient_ReleaseBuffer\n");
        return;
    }
}