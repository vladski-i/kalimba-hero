// A simple introductory program; its main window contains a static picture
// of a triangle, whose three vertices are red, green and blue.  The program
// illustrates viewing with default viewing parameters only.

#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "constants.h"
#include "draw_util.h"
#include "midi_read.h"

// Tines bounding box
const float left = -1;
const float right = 1;
const float up = 1;
const float down = -1;

// Tines properties
const int N_TINES = 17;
const float tine_width = (right - left) / N_TINES;

//Note properties
const float threshold = -0.8;
const float note_speed_multiplier = 0.01;

note notes[17];

//Song properties
const float BPM = 120.0;

char tines[17] = {'D', 'B', 'G', 'E', 'C', 'A', 'F', 'D', 'C', 'E', 'G', 'B', 'D', 'F', 'A', 'C', 'E'};

//clock
uint64_t time = 0;

// Clears the current window and draws a triangle.
void display()
{

    // Set every pixel in the frame buffer to the current clear color.
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < 17; i++)
        draw_note(notes[i], time);

    drawLegend();
    // drawAxes();
    drawTines();
    drawThreshold();

    // Flush drawing command buffer to make drawing happen as soon as possible.
    glFlush();
}

void timer(int v)
{
    time++;
    printf("time is %ld\n", time);
    glutPostRedisplay();
    glutTimerFunc(1000.0 / 60.0, timer, v);
}

void reshape(GLint w, GLint h)
{
    glViewport(0, 0, w, h);
    GLfloat aspect = (GLfloat)w / (GLfloat)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (float)w / (float)h, 1.0, 150.0);
    // Must be in GL_MODELVIEW mode and load identity first
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 4.0f,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);
}
// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
int main2(int argc, char **argv)
{

    // Use a single buffered window in RGB mode (as opposed to a double-buffered
    // window or color-index mode).
    glutInit(&argc, argv);
    glfwInit();

    // Get primary monitor and dimensions
    int count;
    int monitor_height = 0;
    int monitor_width = 0;
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    if (monitors == NULL)
    {
        printf("Null monitors");
        const char *description;
        int code = glfwGetError(&description);

        if (description)
            printf("%d %s\n", code, description);
    }
    else
    {
        const GLFWvidmode const *mode = glfwGetVideoMode(monitors[0]);
        printf("%d x %d", mode->height, mode->width);
        monitor_height = mode->height;
        monitor_width = mode->width;
    }

    // Init window
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowPosition(80, 80);
    glutInitWindowSize(monitor_width, monitor_height);
    float aspect_ratio = 1.0f * monitor_width / monitor_height;
    printf("aspect ratio: %.3f", aspect_ratio);
    glutCreateWindow("Kalimba Hero");

    // Tell GLUT that whenever the main window needs to be repainted that it
    // should call the function display().
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(10.0, (float)monitor_width / (float)monitor_height, 1.0, 150.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(1.0f, 1.0f, 4.0f,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //setup ticking time
    for (int i = 0; i < 17; i++)
    {
        notes[i].enter_time = 30 * i;
        notes[i].lane = i;
    }
    time = 0;
    glutTimerFunc(1000.0 / 60.0, timer, 0);

    // Tell GLUT to start reading and processing events.  This function
    // never returns; the program only exits when the user closes the main
    // window or kills the process.
    glutMainLoop();
}

int main()
{
    read_midi_file("assets/chpn_op27_2.mid");
    return 0;
}