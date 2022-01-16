// A simple introductory program; its main window contains a static picture
// of a triangle, whose three vertices are red, green and blue.  The program
// illustrates viewing with default viewing parameters only.

#include <GL/glut.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <GL/glu.h>

typedef struct s_note
{
    int lane;
    int enter_time;
} note;

int N_TINES = 17;

char tines[17][1] = {"D", "B", "G", "E", "C", "A", "F", "D", "C", "E", "G", "B", "D", "F", "A", "C", "E"};

void drawCircle(float cx, float cy, float radius, int num_segments)
{
    glBegin(GL_LINE_LOOP);
    for (int ii = 0; ii < num_segments; ii++)
    {
        float theta = 2.0f * 3.1415926f * (float)ii / (float)num_segments; //get the current angle

        float x = radius * cos(theta); //calculate the x component
        float y = radius * sin(theta); //calculate the y component

        glVertex3f(x + cx, y + cy, 0); //output vertex
    }
    glEnd();
}

// Clears the current window and draws a triangle.
void display()
{

    // Set every pixel in the frame buffer to the current clear color.
    glClear(GL_COLOR_BUFFER_BIT);

    //Draw a floor

    // glColor3f(1.0, 1.0, 1.0);
    // glBegin(GL_LINES);
    // for (GLfloat i = -2.5; i <= 2.5; i += 0.25)
    // {
    //     glVertex3f(i, 0, 2.5);
    //     glVertex3f(i, 0, -2.5);
    //     glVertex3f(2.5, 0, i);
    //     glVertex3f(-2.5, 0, i);
    // }
    // glEnd();

    //Draw axes
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(10, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 10, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 10);
    glEnd();

    // Drawing is done by specifying a sequence of vertices.  The way these
    // vertices are connected (or not connected) depends on the argument to
    // glBegin.  GL_POLYGON constructs a filled polygon.
    // glBegin(GL_POLYGON);
    // glColor3f(1, 0, 0);
    // glVertex3f(-0.6, -0.75, 0.5);
    // glColor3f(0, 1, 0);
    // glVertex3f(0.6, -0.75, 0);
    // glColor3f(0, 0, 1);
    // glVertex3f(0, 0.75, 0);
    // glEnd();

    float left = -1;
    float right = 1;
    float up = 1;
    float down = -1;
    float tine_width = (right - left) / N_TINES;
    glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    // printf("Starting at %.3f\n, with increment %.3f", right - left, (right - left) / N_TINES);
    for (float i = left; i < right - 0.1; i += tine_width)
    {
        printf("Drawing line with x %.3f\n", i);
        glVertex3f(i, up, 0);
        glVertex3f(i, down, 0);
    }
    glEnd();
    printf("glEnd()\n");

    //draw one circle in the first lane
    drawCircle(left + tine_width / 2, up - tine_width / 2, tine_width / 2, 30);
    // Flush drawing command buffer to make drawing happen as soon as possible.
    glFlush();
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
int main(int argc, char **argv)
{

    // Use a single buffered window in RGB mode (as opposed to a double-buffered
    // window or color-index mode).
    glutInit(&argc, argv);
    glfwInit();
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
        const GLFWvidmode *mode = glfwGetVideoMode(monitors[0]);
        printf("%d x %d", mode->height, mode->width);
        monitor_height = mode->height;
        monitor_width = mode->width;
    }
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // Position window at (80,80)-(480,380) and give it a title.
    glutInitWindowPosition(80, 80);
    glutInitWindowSize(monitor_width, monitor_height);
    float aspect_ratio = 1.0f * monitor_width / monitor_height;
    printf("aspect ratio: %.3f", aspect_ratio);
    // // gluOrtho2D(-0.5f * aspect_ratio, 0.5f * aspect_ratio, 0.0f, 1.0f);
    // glViewport(0, 0, monitor_width, monitor_height);
    // glOrtho(0.0, monitor_height, 0.0, monitor_width, 0.0, aspect_ratio);
    glutCreateWindow("Kalimba Hero");

    // Tell GLUT that whenever the main window needs to be repainted that it
    // should call the function display().
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // gluPerspective(70.0f, 800.0f / 600.0f, 1.0f, 10000.0f);
    gluPerspective(10.0, (float)monitor_width / (float)monitor_height, 1.0, 150.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // glTranslatef(position_.x, position_.y, position_.z);
    gluLookAt(1.0f, 1.0f, 4.0f,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Tell GLUT to start reading and processing events.  This function
    // never returns; the program only exits when the user closes the main
    // window or kills the process.
    glutMainLoop();
}