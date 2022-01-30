#include "midi_parser.h"
#include <stdio.h>

void read_midi_file(char *path)
{
    MidiParser *parser = parseMidi(path, true, true);
    printf("Nb of tracks %d\n", parser->nbOfTracks);
}