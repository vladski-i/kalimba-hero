#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#include <stdint.h>

typedef struct s_note
{
    int lane;
    int enter_time;
} note;

// Tines bounding box
extern const float left;
extern const float right;
extern const float up;
extern const float down;

// Tines properties
extern const int N_TINES;
extern const float tine_width;

//Note properties
extern const float threshold;
extern const float note_speed_multiplier;

extern char tines[17][1];

#endif
