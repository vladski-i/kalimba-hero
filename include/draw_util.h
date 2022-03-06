#ifndef __DRAW_UTIL_H
#define __DRAW_UTIL_H
#include "constants.h"
#include "midi_parser.h"

note_status draw_note(note n, uint64_t time_in_frames);
void drawAxes();
void drawTines();
void drawThreshold();
void drawLegend(tine *tines);
void draw_current_notes(pitch *notes, uint32_t notes_no);
tine *setup_tines();
#endif