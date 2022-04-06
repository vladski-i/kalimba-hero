#include <complex.h>
#include <jack/types.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "proc.h"
// TODO: implement fft
void dft(uint32_t nframes, float *indata, float *outdata) {
  // printf("dft()\n");
  for (uint32_t k = 0; k < nframes; k++) {
    double complex sumcomplex = 0;
    for (uint32_t t = 0; t < nframes; t++) {
      double fi = 2 * M_PI * t * k / nframes;
      float temp = indata[t];
      // sumcomplex += temp * cexp(-fi * I);
      temp = temp * cexp(-fi * I);
      sumcomplex += temp;
    }
    outdata[k] = cabs(sumcomplex);
  }
}