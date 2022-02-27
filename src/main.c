#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#define CONSTANTS_IMPL
#include "constants.h"
#include "draw_util.h"
#include "midi_read.h"
#include "tui.h"

// clock in ms
uint64_t time_c = 0;

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

void display() {
    // Set every pixel in the frame buffer to the current clear color.
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = note_index; i < notes_no; i++) {
        note_status status = draw_note(notes[i], time_c);
        printf(
            "note index %d lane %d enter time_c %d at time_c %.3f status %d\n",
            i, notes[i].lane, notes[i].enter_time, time_c * tpf, status);
        static_assert(STATES_NO == 3, "Handle all note statuses in display()");
        if (status == WAITING) {
            break;
        } else if (status == IN_FLIGHT) {
            ;
        } else if (status == DONE) {
            note_index++;
        }
    }

    drawLegend(tines);
    // drawAxes();
    drawTines();
    drawThreshold();

    // Flush drawing command buffer to make drawing happen as soon as possible.
    glFlush();
}

void timer(int v) {
    struct timeval current;
    gettimeofday(&current, NULL);
    uint64_t current_usec = current.tv_sec * 1000000 + current.tv_usec;
    time_c = ((current_usec - start_usec) / 1000000.0) * 60;
    if (time_c % 60 == 0) printf("time_c is %ld\n", time_c);
    glutPostRedisplay();
    glutTimerFunc(1000.0 / 60.0, timer, v);
}

void reshape(GLint w, GLint h) {
    glViewport(0, 0, w, h);
    GLfloat aspect = (GLfloat)w / (GLfloat)h;
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
    uint32_t track_number = 0;
    char *midi_file_name = tui(&track_number);
    if (!midi_file_name) {
        fprintf(stderr, "failed to select file");
        exit(1);
    }
    printf("Selected %s track %p\n", midi_file_name, &track_number);
    tines = setup_tines();
    // TODO unhardcode midi file
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

        if (description) printf("%d %s\n", code, description);
    } else {
        const GLFWvidmode const *mode = glfwGetVideoMode(monitors[0]);
        printf("%d x %d", mode->height, mode->width);
        monitor_height = mode->height;
        monitor_width = mode->width;
    }

    // Init window
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowPosition(80, 80);
    glutInitWindowSize(monitor_width, monitor_height);
    glutCreateWindow("Kalimba Hero");

    // Tell GLUT that whenever the main window needs to be repainted that it
    // should call the function display().
    glutDisplayFunc(display);
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
    time_c = 0;
    glutTimerFunc(1000.0 / 60.0, timer, 0);
    printf("First note on lane: %d\n", notes[0].lane);
    printf("C0 lane is %d", C1);

    // get the time in usec right before starting the main loop
    struct timeval start;
    gettimeofday(&start, NULL);
    start_usec = start.tv_sec * 1000000 + start.tv_usec;

    // Start the main loop
    glutMainLoop();
}