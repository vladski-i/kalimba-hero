#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "comb.h"
#include "mem.h"
#include "kiss_fft.h"
//comb text file format:
//N : int -> no of teeth
//next N lines: teeth as integer
//next N lines: thresholds as float
comb *from_file(const char *path){
    comb *retval= alloc(1, sizeof(comb));
    FILE *f= fopen(path, "r");
    fscanf(f, "%u\n", &retval->n);
    retval->teeth = alloc(retval->n, sizeof(float));
    retval->thresh = alloc(retval->n, sizeof(float));
    for(uint i = 0; i < retval->n; i++){
        fscanf(f, "%u\n", &retval->teeth[i]);
    }
    for(uint i = 0; i < retval->n; i++){
        fscanf(f, "%f\n", &retval->thresh[i]);
    }
    return retval;
}

//go through the comb tooth by tooth and check if the fft spectrum cos magnitute is lower than the threshold
//TODO: reinforce the logic, make the activation not depend on a simple threshold, implement fuzzyness and activation value
bool check_samples_against_comb(float *spectrum, uint32_t winsize, comb *c){
    for (uint i = 0; i < c->n; i++){
        if (spectrum[c->teeth[i]] < c->thresh[i]){
            return false;
        }
    }
    return true;
}

bool check_kiss_samples_against_comb(kiss_fft_cpx *spectrum, uint32_t winsize, comb *c){
    for (uint i = 0; i < c->n; i++){
        kiss_fft_cpx sample = spectrum[c->teeth[i]];
        if (sample.r * sample.r - sample.i * sample.i  < c->thresh[i]){
            return false;
        }
    }
    return true;
}

void display_comb(comb *c){
    printf("Displaying comb:\n");
    printf("%u\n", c->n);
    for (uint i = 0; i < c->n; i++){
        printf("%u\n", c->teeth[i]);
    }
    for (uint i = 0; i < c->n; i++){
        printf("%f\n", c->thresh[i]);
    }
}