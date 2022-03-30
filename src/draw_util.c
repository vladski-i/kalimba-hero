#include "draw_util.h"

#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "constants.h"
#include "mem.h"
#include "midi_parser.h"
#include "midi_read.h"

extern uint64_t time_in_frames;

static note_status drawCircle(float cx, float cy, float radius,
                              int num_segments) {
  if (cy > threshold) {
    glBegin(GL_LINE_LOOP);
    glColor3f(1, 1, 1);
    for (int ii = 0; ii < num_segments; ii++) {
      float theta = 2.0f * 3.1415926f * (float)ii /
                    (float)num_segments; // get the current angle

      float x = radius * cos(theta); // calculate the x component
      float y = radius * sin(theta); // calculate the y component

      glVertex3f(x + cx, y + cy, 0); // output vertex
    }
    glEnd();
    return IN_FLIGHT;
  }
  return DONE;
}

note_status draw_note(note n, const uint64_t time_in_frames) {
  if (time_in_frames * tpf > n.enter_time) {
    return drawCircle(left + n.lane * tine_width + tine_width / 2,
                      up - (1.0f * time_in_frames - (n.enter_time / tpf)) *
                               note_speed_multiplier,
                      tine_width / 2, 30);
  }
  return WAITING;
}

void drawAxes() {
  // Draw axes
  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(10, 0, 0);
  glColor3f(0, 1, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 10, 0);
  glColor3f(0, 0, 1);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 10);
  glEnd();
}

// Draw the lanes for the tines, bounded by the global left, right, up, down
void drawTines() {
  glBegin(GL_LINES);
  glColor3f(1, 1, 1);
  for (float i = left; i <= right; i += tine_width) {
    glVertex3f(i, up, 0);
    glVertex3f(i, down, 0);
  }
  glEnd();
}

void drawThreshold() {
  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3f(left, -0.8, 0);
  glVertex3f(right, -0.8, 0);
  glEnd();
}

// Draw the legend on the tines
void drawLegend(tine *tines) {
  int index = 0;
  for (float i = left; i < right - 0.1; i += tine_width, index++) {
    if (index % 3 != 2)
      glColor3f(1.0, 1.0, 1.0);
    else
      glColor3f(0.0, 0.0, 1.0);

    // printf("drawing %c at %.3f, %.3f\n", tines[index], left + i *
    // tine_width + tine_width / 2, up);
    glRasterPos2f(left + index * tine_width + tine_width / 2 - tine_width / 4,
                  up - tine_width / 2);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, tines[index].name[0]);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, tines[index].name[1]);
    glRasterPos2f(left + index * tine_width + tine_width / 2 - tine_width / 4,
                  down + tine_width / 2);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, tines[index].name[0]);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, tines[index].name[1]);
  }
}

// Creates the array of tines, in the order they would be seen on the kalimba
tine *setup_tines() {
  tine *tines = alloc(N_TINES, sizeof(tine));
  if (tines == NULL) {
    fprintf(stderr, "Unable to allocate memory for array of pitches");
    exit(1);
  }
  static_assert(TINES_NO == 17, "Handle all tines in setup_tines()");
  for (int i = 0; i < N_TINES; i++) {
    tines[i].index = i;
    tines[i].name = tine_names[i];
    tines[i].pitch = pitch_lookup[i];
  }
  return tines;
}

void draw_current_notes(pitch *notes, uint32_t notes_no) {
  glColor3f(1, 0, 0);
  for (uint i = 0; i < notes_no; i++) {
    double x1 = left + notes[i] * tine_width;
    double x2 = left + (notes[i] + 1) * tine_width;
    double y1 = threshold;
    double y2 = threshold + tine_width;
    glRectd(x1, y1, x2, y2);
  }
}

void draw_audio(float *samples, uint32_t samples_no) {
  glColor3f(1, 0, 0);

  return;
}