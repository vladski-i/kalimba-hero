#include <assert.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "constants.h"
#include "mem.h"
#include "midi_parser.h"
#include "midi_read.h"

// TODO unhardcode max midi files
static const int max_midi_files = 100;
static const char *midi_suffix = ".mid";
static const int midi_suffix_len = 4;

typedef enum selection_e { MIDI_FILE, TRACK, SELECTIONS_NO } selection;

static char *qualify_midi_path(char *filename) {
    char *qualified_path =
        (char *)alloc(strlen(filename) + strlen(MIDI_DIR) + 1, sizeof(char));
    sprintf(qualified_path, "%s%s", MIDI_DIR, filename);
    printf("Qualified path to %s\n", qualified_path);
    return qualified_path;
}

static char **get_midi_files(int *len) {
    assert(strlen(midi_suffix) == midi_suffix_len);
    DIR *dp;
    char **file_names = alloc(max_midi_files, sizeof(char *));
    uint8_t idx = 0;
    struct dirent *ep;
    dp = opendir(MIDI_DIR);

    if (dp != NULL) {
        while ((ep = readdir(dp))) {
            if (!strcmp(ep->d_name + strlen(ep->d_name) - midi_suffix_len,
                        midi_suffix))
                file_names[idx++] = strdup_m(ep->d_name);
        }
        closedir(dp);
    } else {
        fprintf(stderr, "Could not open midi directory\n");
        exit(1);
    }
    *len = idx;
    return file_names;
}

// The TUI allows for browsing midi files and their tracks. Use the arrow keys
// to move, ENTER to select file/track and Q to return to file selection or quit
// the program.
char *tui(uint32_t *track_number) {
    static char *unnamed_track = "Unnamed track";
    int files_no = 0;
    char **midi_files = get_midi_files(&files_no);
    MidiParser **parsed_files = alloc(files_no, sizeof(MidiParser *));
    for (int i = 0; i < files_no; i++) {
        MidiParser *parser =
            copy(parseMidi(qualify_midi_path(midi_files[i]), false, false));
        printf("%p -> %d\n", (void *)parser, parser->nbOfTracks);
        *(parsed_files + i) = parser;
    }
    printf("%s vs %s vs %p\n", parsed_files[0]->tracks[1].name,
           parsed_files[1]->tracks[1].name, (void *)parsed_files);
    initscr();
    noecho();
    cbreak();

    int xmax = getmaxx(stdscr);

    WINDOW *filewin = newwin(files_no + 3, xmax - 12, 8, 5);
    WINDOW *trackwin = newwin(20, xmax - 12, 8 + files_no + 5, 5);

    refresh();

    box(filewin, 0, 0);
    wrefresh(filewin);
    wrefresh(trackwin);

    keypad(filewin, true);

    int key = 0;
    int sel_file = 0;
    int sel_track = 0;
    selection sel = MIDI_FILE;

    while (true) {
        // file window setup
        mvwprintw(filewin, 0, 1, "Select a file %d", sel_file);
        for (int i = 0; i < files_no; i++) {
            if (sel_file == i) wattron(filewin, A_REVERSE);
            mvwprintw(filewin, i + 2, 1, "%s %d", midi_files[i],
                      parsed_files[i]->nbOfTracks);
            wattroff(filewin, A_REVERSE);
        }
        wclear(trackwin);
        box(trackwin, 0, 0);
        // track window setup
        mvwprintw(trackwin, 0, 1, "%s tracks %d", midi_files[sel_file],
                  sel_track);
        for (int i = 0; i < parsed_files[sel_file]->nbOfTracks; i++) {
            if (sel_track == i) wattron(trackwin, A_REVERSE);
            char *name = parsed_files[sel_file]->tracks[i].name;
            if (!name) {
                name = unnamed_track;
            }
            mvwprintw(trackwin, i + 2, 1, name);
            wattroff(trackwin, A_REVERSE);
        }
        wrefresh(trackwin);

        key = wgetch(filewin);
        switch (sel) {
            case MIDI_FILE:
                switch (key) {
                    case KEY_UP:
                        sel_file =
                            (sel_file == 0 ? (files_no - 1) : (sel_file - 1)) %
                            files_no;
                        sel_track = 0;
                        break;
                    case KEY_DOWN:
                        sel_file = (sel_file + 1) % files_no;
                        sel_track = 0;
                        break;
                    case 'q':
                        exit(0);
                    default:
                        break;
                }
                if (key ==
                    '\n') {  // KEY_ENTER is something else, not working here
                    sel = TRACK;
                    break;
                }
                break;
            case TRACK:;  // empty statement to allow delcaration of a new
                          // variable
                uint32_t tracks_no = parsed_files[sel_file]->nbOfTracks;
                switch (key) {
                    case KEY_UP:
                        sel_track = (sel_track == 0 ? (tracks_no - 1)
                                                    : (sel_track - 1)) %
                                    tracks_no;
                        break;
                    case KEY_DOWN:
                        sel_track = (sel_track + 1) % tracks_no;
                        break;
                    case 'q':
                        sel = MIDI_FILE;
                        break;
                    default:
                        break;
                }
                if (key == '\n') {
                    // KEY_ENTER is something else, not working here
                    *track_number = sel_track;
                    endwin();
                    printf("Selecting %d\n", *track_number);
                    return qualify_midi_path(midi_files[sel_file]);
                    break;
                }
                break;
            default:
                break;
        }
    }
}