#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

typedef struct IMMDevice IMMDevice;


// TODO: Organise all this later.
typedef struct 
{
    uint32_t cursor;
    uint32_t size;

    float* data;
} sound_t;

// TODO: Some function will need to parse the wav into the sound_t

// TODO: Need to actually plan the API.


// TODO: Store vector of playing sounds.
typedef struct
{
    IMMDevice* device;
} audio_t;


/*

Should set the output device basically.

*/
uint8_t audio_init(audio_t* audio);

void audio_play();

void audio_tick();

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


We want to support potential future file formats. even if we don't actually want
to use them. therefore we need some interface for loading the sound. a sound must
store it's format too. so it can be converted when playing. We could convert on
sound load, but that would mean potentially storing more memory assuming the 
sound is a worse quality than the device??

Need to consider resource loading...

we can load some file format into an audio source.

this audio source will then be played, which essentially creates a sound from it.

for now an audio source can really just support the wav data? can be updated
in the future.

audio_src_from_wav?














*/


#endif