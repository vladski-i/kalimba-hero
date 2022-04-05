#ifndef __PROC_H
#define __PROC_H

#include <stdint.h>

void dft(uint32_t nframes, float *indata, float *outdata);

typedef enum fft_dir {
  FFT_FORWARD, /* kernel uses "-1" sign */
  FFT_INVERSE  /* kernel uses "+1" sign */
} fft_dir;

#endif