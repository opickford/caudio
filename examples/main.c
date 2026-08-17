#include <sfc/audio.h>
#include <sfc/sound.h>

int main()
{
    SFC_Audio* audio = sfc_audio_create();
    if (!audio)
    {
        return -1;
    }

    SFC_Sound* plankton = sfc_sound_read("C:/Users/olive/source/repos/audio/examples/res/plankton.wav", SFC_WAV);
    SFC_Sound* omg = sfc_sound_read("C:/Users/olive/source/repos/audio/examples/res/omg.wav", SFC_WAV);
    SFC_Sound* boing = sfc_sound_read("C:/Users/olive/source/repos/audio/examples/res/boing.wav", SFC_WAV);

    SFC_SoundInstance* p0 = sfc_audio_play(audio, omg);
    SFC_SoundInstance* p1 = sfc_audio_play(audio, plankton);
    SFC_SoundInstance* p2 = sfc_audio_play(audio, boing);

    // Simulate gameloop..
    while (1)
    {
        sfc_audio_tick(audio);
    }

    sfc_audio_destroy(audio);
    
	return 0;
}
