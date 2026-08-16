#include <stdio.h>
#include <stdint.h>

#include "audio/audio.h"
#include "audio/wav.h"

int main()
{
    Wav wave0 = ReadWav("C:/Users/olive/source/repos/audio/examples/res/plankton.wav");
    Wav wave1 = ReadWav("C:/Users/olive/source/repos/audio/examples/res/omg.wav");

    Audio audio;
    if (!audio_init(&audio))
    {
        return -1;
    }

    PrintDeviceName(audio.device);

    Sound plankton = sound_from_wav(&wave0);
    Sound omg = sound_from_wav(&wave1);

    SoundInstance* p0 = audio_play(&audio, &plankton);
    SoundInstance* p1 = audio_play(&audio, &omg);

    // Simulate gameloop..
    while (1)
    {
        audio_tick(&audio);
        Sleep(100);
    }

    audio_destroy(&audio);
    
	return 0;
}
