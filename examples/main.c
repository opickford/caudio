#include <stdio.h>
#include <stdint.h>

#include "audio/audio.h"
#include "audio/wav.h"

int main()
{
    Wav wave0 = ReadWav("C:/Users/olive/source/repos/audio/examples/res/plankton.wav");
    Wav wave1 = ReadWav("C:/Users/olive/source/repos/audio/examples/res/omg.wav");

    printf("wave0: %d\n", wave0.dataSize);

    Audio audio;
    if (!audio_init(&audio))
    {
        return -1;
    }

    PrintDeviceName(audio.device);

    Sound plankton = sound_from_wav(&wave0);
    Sound omg = sound_from_wav(&wave1);

    // TODO: gotta figure out how to play the sounds, we probably want to 
    // convert a wav into a sound straight away, then this can be reused.
    // Also, do we want the audio manager to remove a sound when it's done?
    // what if we had a sound that is frequently played, like a gunshot?
    audio_play(&audio, plankton);
    audio_play(&audio, omg);

    // Simulate gameloop..
    while (1)
    {
        audio_tick(&audio);
        Sleep(10);
    }

    audio_destroy(&audio);
    
	return 0;
}
