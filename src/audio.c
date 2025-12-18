#include "audio/audio.h"

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

// TODO: Define in separate header?
const CLSID CLSID_MMDeviceEnumerator = { 0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E } };
const IID IID_IMMDeviceEnumerator = { 0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 } };
const IID IID_IAudioMeterInformation = { 0xC02216F6, 0x8C67, 0x4B5B, { 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64 } };

const IID IID_IAudioClient = { 0x1CB9AD4C, 0xDBFA, 0x4c32, {0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2} };
const IID IID_IAudioRenderClient = { 0xF294ACFC, 0x3146, 0x4483, {0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2} };

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

// TODO: How do we return success/failure?
uint8_t audio_init(audio_t* audio)
{
    memset(audio, 0, sizeof(audio_t));

    // Initialise COM.
    HRESULT hr = CoInitialize(NULL);

    if (FAILED(hr))
    {
        printf("failed to init COM\n");
        return 0;
    }

    GetDefaultDevice(&audio->device);








    return 1;
}