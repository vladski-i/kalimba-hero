#ifndef __DRAW_UTIL_H
#define __DRAW_UTIL_H
#include "constants.h"

void drawCircle(float cx, float cy, float radius, int num_segments);
int draw_note(note n, uint64_t time);
void drawAxes();
void drawTines();
void drawThreshold();
#endif