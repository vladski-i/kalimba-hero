#ifndef __COMB_H
#define __COMB_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "kiss_fft.h"

typedef struct comb_s{
    uint n;
    uint *teeth;
    float *thresh;
    void *data;
}comb;

comb *from_file(const char *path);
void display_comb(comb *c);
bool check_kiss_samples_against_comb(kiss_fft_cpx *spectrum, uint32_t winsize, comb *c);

#endif