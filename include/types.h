#ifndef __TYPES_H
#define __TYPES_H

#include <stdint.h>

typedef struct s_note {
    int lane;
    int enter_time;
} note;

typedef struct s_tine {
    char *name;
    uint8_t pitch;
    uint8_t index;

} tine;

typedef enum note_status_e {
    WAITING,
    IN_FLIGHT,
    DONE,
    STATUSES_NO
} note_status;

// Number represents number of dots above the note
// TODO: unhardcode tine midi values
#ifndef PIANO
typedef enum tine_values_e {
    D2,
    B1,
    G1,
    E1,
    C1,
    A0,
    F0,
    D0,
    C0,
    E0,
    G0,
    B0,
    D1,
    F1,
    A1,
    C2,
    E2,
    TINES_NO
} pitch;
#else
// notes arranged as they would be on an all white keys piano (easier for
// debugging)
typedef enum tine_values_e {
    C0,
    D0,
    E0,
    F0,
    G0,
    A0,
    B0,
    C1,
    D1,
    E1,
    F1,
    G1,
    A1,
    B1,
    C2,
    D2,
    E2,
    TINES_NO
} pitch;
#endif

#endif