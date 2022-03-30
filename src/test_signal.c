#include <stdio.h>
#include <stdlib.h>

#include "proc.h"

#define SAMPLE_RATE 44100

#define LEN 890472

int main(int argc, char **argv) {
  FILE *f = fopen("data.txt", "r");
  jack_default_audio_sample_t *data =
      calloc(LEN, sizeof(jack_default_audio_sample_t));
  for (uint32_t i = 0; i < LEN; i++) {
    fscanf(f, "%f", data + i);
  }
  jack_default_audio_sample_t *out =
      calloc(1024, sizeof(jack_default_audio_sample_t));
  dft(1024, data, out);
  for (uint32_t i = 0; i < 1024; i++) {
    printf("%.3f\n", *(out + i));
  }
}