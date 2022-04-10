#include <bits/stdint-uintn.h>
#include <jack/jack.h>
#include <jack/ringbuffer.h>
#include <jack/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "engine.h"
#include "kiss_fft.h"
#include "mem.h"
#include "proc.h"
#include "types.h"
#include "comb.h"

// Assume no more than 16 concurrent notes can play at a single time
#define MAX_CONCURRENT_NOTES 16
// Assume jack window size is set to 1024 for now
// #TODO: unhardcode winsize
#define WINSIZE 1024

typedef struct client_data_s {
  jack_ringbuffer_t *rb;
  jack_ringbuffer_t *audio_data;
  jack_ringbuffer_t *trigger_activations;
  pitch mock_note;
  uint32_t period;
  jack_default_audio_sample_t *hamming_win; // precomputed to save time
  uint32_t triggers_no;
  comb **triggers;
  bool *activations;
} client_data;

static jack_client_t *client;
static jack_port_t *input_port;
static client_data *data;
float *poll_data;
static kiss_fft_cfg cfg;

static void jack_shutdown(void *arg) {
  end_mem();
  exit(1);
}

// dummy  callback, produces notes in sequence for 30 windows at a time
static int process_callback(jack_nframes_t nframes, void *arg) {
  client_data *data = (client_data *)arg;
  jack_ringbuffer_write(data->rb, (char *)&(data->mock_note),
                        1 * sizeof(pitch));
  jack_default_audio_sample_t *window =
      (jack_default_audio_sample_t *)jack_port_get_buffer(input_port, nframes);
  kiss_fft_cpx *time_samples = alloc(1024, sizeof(kiss_fft_cpx));
  kiss_fft_cpx *spectrum = alloc(1024, sizeof(kiss_fft_cpx));
  for (uint i = 0; i < nframes; i++)
    time_samples[i].r =
        window[i] * (0.54 - 0.46 * cos((2.0f * M_PI * i) / 1023));
  kiss_fft(cfg, time_samples, spectrum);
  for (uint i = 0; i < data->triggers_no; i++){
    data->activations[i] = check_kiss_samples_against_comb(spectrum, nframes, data->triggers[i]);
  }
  jack_ringbuffer_write(data->trigger_activations, (char *) data->activations, data->triggers_no * sizeof(bool));
  jack_ringbuffer_write(data->audio_data, (char *)window, nframes * 4);
  data->period = (data->period + 1) % 30;
  if (!data->period) {
    data->mock_note = (data->mock_note + 1) % TINES_NO;
  }
  return 0;
}

// initialize jack client and port
engine_status engine_init(comb **comb_arr, uint32_t comb_arr_len) {
  cfg = kiss_fft_alloc(WINSIZE, 0, NULL, NULL);
  poll_data = alloc(WINSIZE, sizeof(float));
  data = alloc(1, sizeof(client_data));
  data->triggers = comb_arr;
  data->triggers_no = comb_arr_len;
  data->activations = alloc(comb_arr_len, sizeof(bool));
  data->hamming_win = alloc(WINSIZE, sizeof(jack_default_audio_sample_t));
  for (uint i = 0; i < WINSIZE; i++) {
    data->hamming_win[i] = 0.54 - 0.46 * cos((2 * M_PI * i) / (WINSIZE - 1));
  }
  data->rb = jack_ringbuffer_create(MAX_CONCURRENT_NOTES * sizeof(pitch) +
                                    1); // +1 for the end byte
  data->trigger_activations = jack_ringbuffer_create(comb_arr_len * sizeof(bool) + 1);
  // Allocate a windows' worth of audio data
  data->audio_data = jack_ringbuffer_create(
      WINSIZE * sizeof(jack_default_audio_sample_t) + 1); // +1 for the end byte
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
  // printf("engine_poll_notes()\n");
  pitch *current_notes = alloc(MAX_CONCURRENT_NOTES, sizeof(pitch));
  uint32_t bytes_read = jack_ringbuffer_read(
      data->rb, (char *)current_notes, MAX_CONCURRENT_NOTES * sizeof(pitch));
  if (bytes_read % sizeof(pitch)) {
    fprintf(stderr, "Trailing bytes in the notes ringbuffer!\n");
  }
  *notes_read = bytes_read / sizeof(pitch);
  return current_notes;
}

bool *engine_poll_triggers(uint32_t *triggers_read) {
  // printf("engine_poll_triggers()\n");
  bool *current_triggers = alloc(data->triggers_no, sizeof(bool));
  uint32_t bytes_read = jack_ringbuffer_read(data->trigger_activations, (char *)current_triggers,
                                             data->triggers_no * sizeof(bool));
  if (bytes_read % sizeof(bool)) {
    fprintf(stderr, "Trailing bytes in the triggers ringbuffer!\n");
    return NULL;
  }
  *triggers_read = bytes_read / sizeof(bool);
  return current_triggers;
}

float *engine_poll_data(uint32_t *samples_no) {
  // printf("engine_poll_data()\n");
  float *bytes = alloc(WINSIZE, sizeof(float));
  uint32_t bytes_read = jack_ringbuffer_read(data->audio_data, (char *)bytes,
                                             WINSIZE * sizeof(float));
  if (bytes_read % sizeof(float)) {
    fprintf(stderr, "Trailing bytes in the audio ringbuffer!\n");
    return NULL; // skip drawing for the frame if trailing bytes are detected;
                 // may lead to issues
  }
  *samples_no = bytes_read / sizeof(float);
  return bytes;
}

engine_status engine_stop() {
  if (jack_deactivate(client)) {
    fprintf(stderr, "Failed to deactivate jack client\n");
    return ENGINE_ERROR;
  }
  if(jack_client_close(client)) {
    fprintf(stderr, "Failed to close jack client\n");
    return ENGINE_ERROR;
  }
  return ENGINE_STOPPED;
}