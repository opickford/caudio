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


    audio_play(&audio, wave0);

    while (1)
    {
        audio_tick(&audio);
    }

    audio_destroy(&audio);
    
	return 0;
}
