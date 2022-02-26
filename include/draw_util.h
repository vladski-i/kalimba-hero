#ifndef __DRAW_UTIL_H
#define __DRAW_UTIL_H
#include "constants.h"
#include "midi_parser.h"

note_status draw_note(note n, uint64_t time_c);
void drawAxes();
void drawTines();
void drawThreshold();
void drawLegend(tine *tines);
tine *setup_tines();
#endif