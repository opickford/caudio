#include "sound.h"
#include "sound_internal.h"

#include "formats/wav.h"

#include <malloc.h>

SFC_Sound* sfc_sound_read(const char* filename, SFC_FileType filetype)
{
    switch (filetype)
    {
    case SFC_WAV:
    {
        // TODO: should we need to chain calls like this?
        Wav wav = sfc_wav_read(filename);
        return sfc_sound_from_wav(&wav);
    }
    break;
    }

    // TODO: handle some error? impossible to get here given full switch.
    return 0;
}

void sfc_sound_destroy(SFC_Sound* sound)
{
    if (sound->data) free(sound->data);
    sound->data = 0;

    free(sound);
}

