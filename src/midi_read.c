#include "midi_parser.h"
#include "constants.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

// TODO unhardcode track number
#define TRACK_NO 0
#define START_DELAY 100

note *parser_to_interpreter_note(Note parser_note)
{
    if (parser_note.velocity == 0)
        return NULL;
    note *n = malloc(sizeof(note));
    n->enter_time = parser_note.timeBeforeAppear;
    for (uint8_t lane = 0; lane <= N_TINES; lane++)
    {
        if (pitch_lookup[lane] == parser_note.pitch)
        {
            n->lane = lane;
            printf("Found note on lane %d\n", n->lane);
            return n;
        }
    }
    fprintf(stderr, "Unknown note, pitch %d\n", parser_note.pitch);
    return NULL;
}

// notes_no returned by side efect
note *read_midi_file(char *path, uint32_t *notes_no)
{
    MidiParser *parser = parseMidi(path, false, true);
    if (!parser)
    {
        fprintf(stderr, "failed to parse midi file %s\n", path);
        exit(1);
    }
    printf("Nb of tracks %d\n", parser->nbOfTracks);
    uint32_t us_per_quarter = 0;
    for (uint8_t i = 0; i < parser->nbOfTracks; i++)
    {
        Track *track = parser->tracks + i;
        for (uint32_t j = 0; j < track->nbOfEvents; j++)
        {
            if (track->events[j].type == MidiTempoChanged)
            {
                us_per_quarter = *(int *)track->events[j].infos;
            }
        }
    }
    printf("us_per_quarter: %d\n", us_per_quarter);
    printf("Ticks: %d\n", (int)parser->ticks);
    Track *track = parser->tracks + TRACK_NO;
    printf("notes: %d\n", track->nbOfNotes);
    *notes_no = track->nbOfNotes;

    note *filtered = calloc(*notes_no, sizeof(note));
    uint32_t idx = 0;
    for (uint32_t i = 0; i < *notes_no; i++)
    {

        note *n = parser_to_interpreter_note(track->notes[i]);
        if (n)
        {
            filtered[idx++] = *n;
        }
    }

    printf("First note string is %s\n", getNoteString(track->notes[0].pitch));
    *notes_no = idx;
    printf("Found %d playable notes\n", idx);
    uint32_t first_note_enter = track->notes->timeBeforeAppear;
    bpm = (int)ceil(60 * 1000 * 1000.0f / us_per_quarter);
    printf("BPM: %d\n", bpm);
    tpf = ((bpm * 1.0f) / 60.0 * parser->ticks) / 60.0;
    printf("Ticks per frame: %.3f\n", tpf);
    printf("First note at %lu frames\n", track->notes->timeBeforeAppear);
    return filtered;
}