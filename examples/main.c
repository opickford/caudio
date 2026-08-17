#include <stdio.h>

#include "caudio/audio.h"
#include "caudio/wav.h"

int main()
{
    Wav wave0 = ReadWav("C:/Users/olive/source/repos/audio/examples/res/plankton.wav");
    Wav wave1 = ReadWav("C:/Users/olive/source/repos/audio/examples/res/omg.wav");
    Wav boing_wav = ReadWav("C:/Users/olive/source/repos/audio/examples/res/click.wav");

    Audio audio;
    if (!audio_init(&audio))
    {
        return -1;
    }

    PrintDeviceName(audio.device);

    Sound plankton = sound_from_wav(&wave0);
    Sound omg = sound_from_wav(&wave1);
    Sound boing = sound_from_wav(&boing_wav);

    //SoundInstance* p0 = audio_play(&audio, &plankton);
    SoundInstance* p1 = audio_play(&audio, &omg);
    //SoundInstance* boing_inst = audio_play(&audio, &boing);

    // Simulate gameloop..
    while (1)
    {
        //if (!boing_inst->playing)
        //{
        //    boing_inst = audio_play(&audio, &boing);
        //    printf("Num Instances: %u\n", audio.mixer.num_instances);
        //}

        audio_tick(&audio);
        Sleep(10);
    }

    audio_destroy(&audio);
    
	return 0;
}
