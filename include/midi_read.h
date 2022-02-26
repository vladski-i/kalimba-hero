#ifndef __MIDI_READ_H
#define __MIDI_READ_H
#include "midi_parser.h"
#include "constants.h"

note *read_midi_file(char *path, int *notes_no, int *bpm);
note *parser_to_interpreter_note(Note parser_note);

#endif