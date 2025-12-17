#include <stdio.h>

#define COBJMACROS
#include <initguid.h>
#include <mmDeviceapi.h>
#include <Windows.h>
#include <audioendpoints.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <Audioclient.h>

#include <math.h>

#include <stdint.h>

#include <stdbool.h>

DEFINE_GUID(IDD_IMMDeviceCollection, 0x0bd7a1be, 0x7a1a, 0x44db, 0x83, 0x97, 0xcc, 0x53, 0x92, 0x38, 0x7b, 0x5e);
const CLSID CLSID_MMDeviceEnumerator = { 0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E } };
const IID IID_IMMDeviceEnumerator = { 0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 } };
const IID IID_IAudioMeterInformation = { 0xC02216F6, 0x8C67, 0x4B5B, { 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64 } };

const IID IID_IAudioClient = { 0x1CB9AD4C, 0xDBFA, 0x4c32, {0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2} };
const IID IID_IAudioRenderClient = { 0xF294ACFC, 0x3146, 0x4483, {0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2} };

BOOL AudioInit()
{
    HRESULT hr = CoInitialize(NULL);

    if (FAILED(hr))
    {
        printf("failed to init COM\n");
        return FALSE;
    }

    return TRUE;
}

void AudioRelease()
{
    CoUninitialize();
}

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

void CleanupDevice(IMMDevice* device)
{
    if (device) IMMDevice_Release(device);
    device = NULL;
}

void Play(IMMDevice* device)
{
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

    for (int i = 0; i < bufferFrameCount * pwfx->nBlockAlign; ++i)
    {
        pData[i] = MAXUINT8;
    }

    hr = IAudioRenderClient_ReleaseBuffer(pRenderClient, bufferFrameCount, flags);

    if (FAILED(hr))
    {
        printf("Failed to IAudioRenderClient_ReleaseBuffer\n");
        return;
    }

    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

    hr = IAudioClient_Start(pAudioClient);
    if (FAILED(hr))
    {
        printf("Failed to IAudioClient_Start\n");
        return;
    }

    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

        hr = IAudioClient_GetCurrentPadding(pAudioClient, &numFramesPadding);
        if (FAILED(hr))
        {
            printf("Failed to IAudioClient_GetCurrentPadding\n");
            return;
        }

        numFramesAvailable = bufferFrameCount - numFramesPadding;
        printf("numFramesAvailable: %d\n", numFramesAvailable);

        hr = IAudioRenderClient_GetBuffer(pRenderClient, numFramesAvailable, &pData);

        if (FAILED(hr))
        {
            printf("Failed to IAudioRenderClient_GetBuffer\n");
            return;
        }

        float frequency = 50.0f; // A4
        float amplitude = 0.25f;
        static double theta = 0.0;
        double thetaIncrement = 2.0 * 3.14159 * frequency / pwfx->nSamplesPerSec;

        float* pFloatData = (float*)pData;
        UINT32 frames = numFramesAvailable;
        UINT32 channels = pwfx->nChannels;

        for (UINT32 i = 0; i < frames; ++i)
        {
            float sample = (float)(sin(theta) * amplitude);
            theta += thetaIncrement;

            for (UINT32 ch = 0; ch < channels; ++ch)
            {
                *pFloatData++ = sample;
            }
        }


        hr = IAudioRenderClient_ReleaseBuffer(pRenderClient, numFramesAvailable, flags);

        if (FAILED(hr))
        {
            printf("Failed to IAudioRenderClient_ReleaseBuffer\n");
            return;
        }
    }
}


int main()
{
    if (!AudioInit())
    {
        return -1;
    }

    IMMDevice* device = NULL;
    GetDefaultDevice(&device);
    
    PrintDeviceName(device);

    Play(device);

    CleanupDevice(device);
    
    AudioRelease();

	return 0;
}
