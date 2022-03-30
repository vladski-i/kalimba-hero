#ifndef __PROC_H
#define __PROC_H

#include <jack/types.h>

void dft(jack_nframes_t nframes, jack_default_audio_sample_t *indata,
         jack_default_audio_sample_t *outdata);

#endif