#include <stdio.h>

#include "caudio/audio.h"
#include "caudio/wav.h"

int main()
{
    Wav wave0 = ReadWav("C:/Users/olive/source/repos/audio/examples/res/plankton.wav");
    Wav wave1 = ReadWav("C:/Users/olive/source/repos/audio/examples/res/omg.wav");
    Wav boing_wav = ReadWav("C:/Users/olive/source/repos/audio/examples/res/boing.wav");

    Audio* audio = audio_create();
    if (!audio)
    {
        return -1;
    }

    Sound plankton = sound_from_wav(&wave0);
    Sound omg = sound_from_wav(&wave1);
    Sound boing = sound_from_wav(&boing_wav);

    SoundInstance* p0 = audio_play(audio, &omg);
    SoundInstance* p1 = audio_play(audio, &plankton);
    SoundInstance* p2 = audio_play(audio, &boing);

    // Simulate gameloop..
    while (1)
    {
        audio_tick(audio);
    }

    audio_destroy(audio);
    
	return 0;
}
