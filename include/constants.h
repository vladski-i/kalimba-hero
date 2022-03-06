#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#include <stdint.h>

#include "types.h"
#define N_TINES 17

// TODO unhardcode midi directory
#define MIDI_DIR "./assets/midi/"

#ifndef CONSTANTS_IMPL
// Tines bounding box
extern const float left;
extern const float right;
extern const float up;
extern const float down;

// bpm
extern uint32_t bpm;

extern float tpf;

// Tines properties
extern const float tine_width;

// Note properties
extern const float threshold;
extern const float note_speed_multiplier;

extern char tine_names[17][2];

#endif

extern const int pitch_lookup[N_TINES];

#endif

#ifdef CONSTANTS_IMPL

// Tines bounding box
const float left = -1;
const float right = 1;
const float up = 1;
const float down = -1;

// Tines properties
// TODO: unhardcode number of tines
const float tine_width = (right - left) / N_TINES;

// Note properties
const float threshold = -0.8;
const float note_speed_multiplier = 0.01;

// Song properties
uint32_t bpm = 120;

// Ticks per 60fps frame
float tpf = 0;

#ifndef PIANO
// notes arranged as they would be on a kalimba
char tine_names[TINES_NO][2] = {"D2", "B1", "G1", "E1", "C1", "A0",
                                "F0", "D0", "C0", "E0", "G0", "B0",
                                "D1", "F1", "A1", "C2", "E2"};

#else
// notes arranged as they would be on an all white keys piano (easier for
// debugging)
char tine_names[TINES_NO][2] = {"C0", "D0", "E0", "F0", "G0", "A0",
                                "B0", "C1", "D1", "E1", "F1", "G1",
                                "A1", "B2", "C2", "D2", "E2"};
#endif

#undef CONSTANTS_IMPL

const int pitch_lookup[N_TINES] = {
    [C0] = 60, [D0] = 62, [E0] = 64, [F0] = 65, [G0] = 67, [A0] = 69,
    [B0] = 71, [C1] = 72, [D1] = 74, [E1] = 76, [F1] = 77, [G1] = 79,
    [A1] = 81, [B1] = 83, [C2] = 84, [D2] = 86, [E2] = 88};
#endif