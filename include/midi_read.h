#ifndef __MIDI_READ_H
#define __MIDI_READ_H
#include "constants.h"
#include "midi_parser.h"

note *read_midi_file(char *path, uint32_t track_no, int *notes_no, int *bpm);
note *parser_to_interpreter_note(Note parser_note);
MidiParser *copy(MidiParser *parser);

#endif