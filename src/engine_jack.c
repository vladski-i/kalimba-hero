#include <jack/jack.h>
#include <jack/ringbuffer.h>
#include <jack/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "mem.h"
#include "types.h"

// Assume no more than 16 concurrent notes can play at a single time
#define MAX_CONCURRENT_NOTES 16

typedef struct client_data_s {
    jack_ringbuffer_t *rb;
    pitch mock_note;
    uint32_t period;
} client_data;

static jack_client_t *client;
static jack_port_t *input_port;
static client_data *data;

static void jack_shutdown(void *arg) {
    end_mem();
    exit(1);
}

// dummy  callback, produces notes in sequence for 30 windows at a time
static int process_callback(jack_nframes_t nframes, void *arg) {
    client_data *data = (client_data *)arg;
    jack_ringbuffer_write(data->rb, (char *)&(data->mock_note),
                          1 * sizeof(pitch));
    data->period = (data->period + 1) % 30;
    if (!data->period) {
        data->mock_note = (data->mock_note + 1) % TINES_NO;
    }
    return 0;
}

// initialize jack client and port
engine_status engine_init() {
    data = alloc(1, sizeof(client_data));
    data->rb = jack_ringbuffer_create(MAX_CONCURRENT_NOTES * sizeof(pitch));
    data->mock_note = 0;
    jack_options_t options = JackNullOption;
    jack_status_t status;
    client = jack_client_open("kalimba-hero", options, &status);
    if (!client) {
        fprintf(stderr, "Could not create jack client\n");
        return ENGINE_ERROR;
    }
    if (status & JackServerFailed) {
        fprintf(stderr, "Could not connect to Jack server\n");
        return ENGINE_ERROR;
    }
    if (status & JackServerStarted) {
        fprintf(stderr, "JACK server started\n");
    }
    if (status & JackNameNotUnique) {
        char *client_name = jack_get_client_name(client);
        fprintf(stderr,
                "kalimba-hero name not available, assigned %s\n"
                "Please make sure previous session ended gracefully\n",
                client_name);
    }
    jack_on_shutdown(client, &jack_shutdown, NULL);
    input_port = jack_port_register(client, "mic in", JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsInput, 0);
    return ENGINE_OK;
}

// set the callback function and activate the client
engine_status engine_start() {
    if (jack_set_process_callback(client, process_callback, (void *)data)) {
        fprintf(stderr, "Failed to set jack process callback\n");
        return ENGINE_ERROR;
    }
    if (jack_activate(client)) {
        fprintf(stderr, "Failed to activate jack client\n");
        return ENGINE_ERROR;
    }
    return ENGINE_STARTED;
}

// poll the RT thread for currently playing notes
pitch *engine_poll_notes(uint32_t *notes_read) {
    pitch *current_notes = alloc(MAX_CONCURRENT_NOTES, sizeof(pitch));
    uint32_t bytes_read = jack_ringbuffer_read(
        data->rb, (char *)current_notes, MAX_CONCURRENT_NOTES * sizeof(pitch));
    if (bytes_read % sizeof(pitch)) {
        fprintf(stderr, "Trailing bytes in the ringbuffer!\n");
    }
    *notes_read = bytes_read / sizeof(pitch);
    return current_notes;
}