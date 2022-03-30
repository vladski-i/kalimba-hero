#include <complex.h>
#include <jack/types.h>
#include <math.h>

void dft(jack_nframes_t nframes, jack_default_audio_sample_t *indata,
         jack_default_audio_sample_t *outdata) {
  for (uint32_t k = 0; k < nframes; k++) {
    double complex sumcomplex = 0;
    for (uint32_t t = 0; t < nframes; t++) {
      double fi = 2 * M_PI * t * k / nframes;
      sumcomplex += indata[t] * cexp(-fi * I);
    }
    outdata[k] = cabs(sumcomplex);
  }
}