#include <assert.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "constants.h"
#include "midi_parser.h"

// TODO unhardcode max midi files
static const int max_midi_files = 100;
static const char const *midi_suffix = ".mid";
static const int midi_suffix_len = 4;

static char *qualify_midi_path(char *filename) {
    char *qualified_path =
        calloc(strlen(filename) + strlen(MIDI_DIR), sizeof(char));
    sprintf(qualified_path, "%s%s", MIDI_DIR, filename);
    return qualified_path;
}

static char **get_midi_files(int *len) {
    assert(strlen(midi_suffix) == midi_suffix_len);
    DIR *dp;
    char **file_names = calloc(max_midi_files, sizeof(char *));
    uint8_t idx = 0;
    struct dirent *ep;
    dp = opendir(MIDI_DIR);

    if (dp != NULL) {
        while (ep = readdir(dp)) {
            if (!strcmp(ep->d_name + strlen(ep->d_name) - midi_suffix_len,
                        midi_suffix))
                file_names[idx++] = strdup(ep->d_name);
        }
        closedir(dp);
    } else {
        fprintf(stderr, "Could not open midi directory\n");
        exit(1);
    }
    *len = idx;
    return file_names;
}

char *tui() {
    int files_no = 0;
    char **midi_files = get_midi_files(&files_no);
    MidiParser *map = calloc(files_no, sizeof(MidiParser));
    initscr();
    noecho();
    cbreak();

    int ymax, xmax;
    getmaxyx(stdscr, ymax, xmax);

    WINDOW *filewin = newwin(files_no + 3, xmax - 12, 8, 5);
    WINDOW *trackwin = newwin(6, xmax - 12, 8 + files_no + 5, 5);
    box(filewin, 0, 0);
    box(trackwin, 0, 0);
    refresh();
    wrefresh(filewin);
    wrefresh(trackwin);

    keypad(filewin, true);

    int key = 0;
    int sel = 0;

    while (true) {
        mvwprintw(filewin, 0, 1, "Select a file");
        for (int i = 0; i < files_no; i++) {
            if (sel == i) wattron(filewin, A_REVERSE);
            mvwprintw(filewin, i + 2, 1, midi_files[i]);
            wattroff(filewin, A_REVERSE);
        }
        key = wgetch(filewin);
        switch (key) {
            case KEY_UP:
                sel = (sel == 0 ? (files_no - 1) : (sel - 1)) % files_no;
                break;
            case KEY_DOWN:
                sel = (sel + 1) % files_no;
            default:
                break;
        }
        if (key == '\n') {
            break;
        }
    }
    endwin();

    return qualify_midi_path(midi_files[sel]);
}