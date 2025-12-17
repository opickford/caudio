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


// Note, this way could cause a lot of issues with badly formed files.

typedef struct
{
    uint32_t ChunkID;
    uint32_t ChunkSize;
    uint32_t Format;

} RIFFHEADER;


typedef struct {
    uint32_t Subchunk1ID;
    uint32_t Subchunk1Size;
} WAVEFMT_HEADER;

typedef struct {
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;

    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    
} WAVEFMT;

typedef struct {
    uint32_t SubChunk2ID;
    uint32_t SubChunk2Size;

} DATAHEADER;




BYTE* data = 0;
uint32_t dataSize = 0;
WAVEFMT fmt = { 0 };

void ReadWAV()
{

    // http://soundfile.sapp.org/doc/WaveFormat/

    const char* fileName = "C:/Users/olive/source/repos/audio/res/plankton.wav";

    FILE* f = fopen(fileName, "rb");

    // 

    //WavHeader wh = { 0 };

    // TODO: Validate ids 

    RIFFHEADER rf = { 0 };
    fread(&rf, sizeof(RIFFHEADER), 1, f);

    if (rf.ChunkID != 0x46464952)
    {
        printf("invalid rf chunk id\n");
        return;
    }
    if (rf.Format != 0x45564157)
    {
        printf("invalid rf format\n");
        return;
    }

    // Search for next chunks.
    uint32_t chunkId = 0;
    uint32_t chunkSize = 0;

    while (fread(&chunkId, sizeof(chunkId), 1, f)) 
    {
        fread(&chunkSize, sizeof(chunkSize), 1, f);

        if (chunkId == 0x20746d66)
        {
            fread(&fmt, sizeof(fmt), 1, f);
        }
        else if (chunkId == 0x61746164)
        {
            dataSize = chunkSize;
            data = malloc(dataSize);

            if (!data)
            {
                printf("failed to mallco\n");
                return;
            }

            fread(data, 1, dataSize, f);

            // TODO: Ensure correct number of elements read.
        }
        else
        {
            // unknown chunkId, skip size of chunk. RIFF format supports different chunks basically.
            fseek(f, chunkSize, SEEK_CUR);
        }
    }

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

    printf("Blockalign: %d\n", fmt.BlockAlign);
    for (int i = 0; i < dataSize * fmt.BlockAlign; ++i)
    {
        //printf("%d ", data[i]);
    }
}



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


    if (fmt.NumChannels != pwfx->nChannels || fmt.BitsPerSample != pwfx->wBitsPerSample)
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
    UINT32 totalFrames = dataSize / fmt.BlockAlign;
    
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
        UINT32 frames = numFramesAvailable;
        UINT32 channels = pwfx->nChannels;

        // Input data is 16bits per sample 
        // we need to convert to 32 bits eexpected output.

        // also input data is 2 channel, output is 8/

        int16_t* input = data; 

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
    ReadWAV();


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
