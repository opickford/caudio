#ifndef AUDIO_H
#define AUDIO_H


typedef struct
{
    int a;
} audio_manager;


void audio_init();

void audio_destroy();

/*

API Design:

We want to constantly call some function to write audio bytes to a buffer.

this means we need some buffer ourself that is going to be next played.

so we maintain a buffer, do some playsound function which writes to this buffer

then when audio_tick() or something is called, we actually write to the 
audiorenderclient buffer??

How will this work if the sound does not fit in our buffer? Do we need to grow the 
buffer? Or add the sound src to some 'playing' list where we constantly sample
until nothing left.



struct {
    int cursor; // current sample
    int size;
    void* data;

} Sound

audio manager stores list of current sounds

audio_tick, called as often as possible:
- gets number of available frames
- reads from audio sources the number of available frames
- increments cursor of each current audio source
- mixes them

Note, no need to mix into our own buffer, can do it on demand.










*/


#endif