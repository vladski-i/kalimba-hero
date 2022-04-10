#include <GL/freeglut.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#define CONSTANTS_IMPL
#include "constants.h"
#include "draw_util.h"
#include "engine.h"
#include "kiss_fft.h"
#include "mem.h"
#include "midi_read.h"
#include "proc.h"
#include "tui.h"
#include "comb.h"

// clock in ms
uint64_t time_in_frames = 0;

// time of day when program started in usec
uint64_t start_usec = 0;

// tines
tine *tines;

// array of notes parsed from midi file at startup
note *notes;

// number of notes in above array
uint32_t notes_no;

// index in the array of notes
uint32_t note_index = 0;

float *old_audio;
uint32_t old_samples_no;

bool *old_activations;

kiss_fft_cfg cfg;

float max_spec = 0.0;

comb *test_comb;

void display() {
  // printf("display()\n");
  // Set every pixel in the frame buffer to the current clear color.
  glClear(GL_COLOR_BUFFER_BIT);

  // draw only the notes currently in flight
  for (int i = note_index; i < notes_no; i++) {
    note_status status = draw_note(notes[i], time_in_frames);
    static_assert(STATUSES_NO == 3, "Handle all note statuses in display()");
    if (status == WAITING) {
      break;
    } else if (status == IN_FLIGHT) {
      ;
    } else if (status == DONE) {
      note_index++;
    }
  }
  uint32_t notes_no = 0;
  uint32_t samples_no = 0;
  uint32_t triggers_no = 0;
  pitch *notes = engine_poll_notes(&notes_no);
  float *audio_data = engine_poll_data(&samples_no);
  bool *activations = engine_poll_triggers(&triggers_no);
  bool color = false;
  if (!activations){
    activations = old_activations;
  }
  else{
    old_activations = activations;
  }
  draw_current_notes(notes, notes_no);
  float *drawable_audio = NULL;
  float drawable_sample_count = 0;
  if (audio_data) {
    if (samples_no == 0) {
      drawable_audio = old_audio;
      drawable_sample_count = old_samples_no;
    } else {
      drawable_audio = audio_data;
      drawable_sample_count = samples_no;
      old_audio = audio_data;
      old_samples_no = samples_no;
    }
  }
  if (drawable_audio) {
    kiss_fft_cpx *time_samples = alloc(1024, sizeof(kiss_fft_cpx));
    kiss_fft_cpx *spectrum = alloc(1024, sizeof(kiss_fft_cpx));
    for (uint i = 0; i < drawable_sample_count; i++){
      time_samples[i].r =
          drawable_audio[i] * (0.54 - 0.46 * cos((2.0f * M_PI * i) / 1023));
      time_samples[i].i = 0;
    }
    kiss_fft(cfg, time_samples, spectrum);
    for (uint i = 0; i < 1024; i++) {
      float mag = fabs(spectrum[i].r * spectrum[i].r +
                       spectrum[i].i * spectrum[i].i);
      if (mag > max_spec) {
        max_spec = mag;
      }
    }
    if(activations[0])
      printf("Tooth is activated %d\n", activations[0]);
    kiss_log_scale_boxed_graph(spectrum, drawable_sample_count / 2, -1.97, -1.05, -1, -1);
    log_scale_sparse_boxed_graph(test_comb->n, drawable_sample_count / 2, test_comb->teeth, test_comb->thresh,-1.97, -1.05, -1, -1, activations[0]);
  }
  boxed_graph(drawable_audio, drawable_sample_count, -1.97, -1.03, -1, 0.5);

  // draw supporting infrastructure
  drawTines();
  drawLegend(tines);
  drawThreshold();

  // Flush drawing command buffer to make drawing happen as soon as possible.
  glFlush();
}

void timer(int v) {
  // get current time of day and update time_in_frames with ms passed since
  // the timer started
  struct timeval current;
  gettimeofday(&current, NULL);
  uint64_t current_usec = current.tv_sec * 1000000 + current.tv_usec;
  time_in_frames = ((current_usec - start_usec) / 1000000.0) * 60;
  if (time_in_frames % 60 == 0){
    printf("time_in_frames is %ld\n", time_in_frames);
    // printf("max_spec is %f\n", max_spec);
  }
  // trigger redisplay function
  glutPostRedisplay();
  // set the timer for one frame in the future (@60fps)
  glutTimerFunc(1000.0 / 60.0, timer, v);
}

void reshape(GLint w, GLint h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (float)w / (float)h, 1.0, 150.0);
  // Must be in GL_MODELVIEW mode and load identity first
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}
// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
int main(int argc, char **argv) {
  init_mem();
  test_comb = from_file("assets/combs/test.comb");
  display_comb(test_comb);
  cfg = kiss_fft_alloc(1024, 0, NULL, NULL);
  engine_status status = engine_init(&test_comb, 1);
  if (status != ENGINE_OK) {
    fprintf(stderr, "Engine failed to start with status %d", status);
    end_mem();
    exit(1);
  }
  uint32_t track_number = 0;
  char *midi_file_name = tui(&track_number);
  if (!midi_file_name) {
    fprintf(stderr, "failed to select file");
    exit(1);
  }
  printf("Selected %s track %p\n", midi_file_name, (void *)&track_number);
  tines = setup_tines();
  notes = read_midi_file(midi_file_name, track_number, &notes_no, &bpm);

  // Use a single buffered window in RGB mode (as opposed to a double-buffered
  // window or color-index mode).
  glutInit(&argc, argv);
  glfwInit();

  // Get primary monitor and dimensions
  int count;
  int monitor_height = 0;
  int monitor_width = 0;
  GLFWmonitor **monitors = glfwGetMonitors(&count);
  if (monitors == NULL) {
    printf("Null monitors");
    const char *description;
    int code = glfwGetError(&description);

    if (description)
      printf("%d %s\n", code, description);
  } else {
    const GLFWvidmode *mode = glfwGetVideoMode(monitors[0]);
    printf("%d x %d", mode->height, mode->width);
    monitor_height = mode->height;
    monitor_width = mode->width;
  }

  // Init window
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  glutInitWindowPosition(80, 80);
  glutInitWindowSize(monitor_width, monitor_height);
  glutCreateWindow("Kalimba Hero");
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

  // Tell GLUT that whenever the main window needs to be repainted that it
  // should call the function display().
  glutDisplayFunc(display);
  // Use the reshape function to preserve aspect ratio whenever window is
  // reshaped
  glutReshapeFunc(reshape);

  // Setup perspective and lookAt so aspect ratio is preserved when resizing
  // the window
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(10.0, (float)monitor_width / (float)monitor_height, 1.0,
                 150.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(1.0f, 1.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // setup ticking time
  time_in_frames = 0;
  glutTimerFunc(1000.0 / 60.0, timer, 0);
  printf("First note on lane: %d\n", notes[0].lane);
  printf("C0 lane is %d", C1);

  // get the time in usec right before starting the main loop
  struct timeval start;
  gettimeofday(&start, NULL);
  start_usec = start.tv_sec * 1000000 + start.tv_usec;

  // before starting the main loop, start the engine client's process callback
  // function
  if (engine_start() != ENGINE_STARTED) {
    fprintf(stderr, "Failed to start engine\n");
    end_mem();
    exit(1);
  }

  // Start the main loop
  glutMainLoop();
  printf("Main loop ended\n");
  // clear memory before exiting
  end_mem();
  return 0;
}