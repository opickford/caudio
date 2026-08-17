#ifndef SOUND_H
#define SOUND_H

typedef enum SFC_FileType
{
    SFC_WAV
} SFC_FileType;

typedef struct SFC_Sound SFC_Sound;

SFC_Sound* sfc_sound_read(const char* filename, SFC_FileType filetype);
void sfc_sound_destroy(SFC_Sound* sound);

#endif