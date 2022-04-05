#ifndef __DRAW_UTIL_H
#define __DRAW_UTIL_H
#include "constants.h"
#include "kiss_fft.h"
#include "midi_parser.h"

note_status draw_note(note n, uint64_t time_in_frames);
void drawAxes();
void drawTines();
void drawThreshold();
void drawLegend(tine *tines);
void draw_current_notes(pitch *notes, uint32_t notes_no);
tine *setup_tines();
void boxed_graph(float *samples, uint32_t samples_no, float left, float right,
                 float up, float down);
void kiss_log_scale_boxed_graph(kiss_fft_cpx *samples, uint32_t samples_no, float left,
                      float right, float up, float down);
#endif