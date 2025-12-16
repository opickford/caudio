#include <stdio.h>

#define COBJMACROS
#include <initguid.h>
#include <mmDeviceapi.h>
#include <Windows.h>
#include <audioendpoints.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

#include <stdbool.h>

DEFINE_GUID(IDD_IMMDeviceCollection, 0x0bd7a1be, 0x7a1a, 0x44db, 0x83, 0x97, 0xcc, 0x53, 0x92, 0x38, 0x7b, 0x5e);
const CLSID CLSID_MMDeviceEnumerator = { 0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E } };
const IID IID_IMMDeviceEnumerator = { 0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 } };
const IID IID_IAudioMeterInformation = { 0xC02216F6, 0x8C67, 0x4B5B, { 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64 } };

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

int main()
{
    if (!AudioInit())
    {
        return -1;
    }

    IMMDevice* device = NULL;
    GetDefaultDevice(&device);
    
    PrintDeviceName(device);

    CleanupDevice(device);
    
    AudioRelease();

	return 0;
}
