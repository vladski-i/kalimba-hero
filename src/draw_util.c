#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "draw_util.h"

extern uint64_t time;

void drawCircle(float cx, float cy, float radius, int num_segments)
{
    if (cy > threshold)
    {
        printf("Drawing circle at %.3f, %.3f\n", cx, cy);
        glBegin(GL_LINE_LOOP);
        glColor3f(1, 1, 1);
        for (int ii = 0; ii < num_segments; ii++)
        {
            float theta = 2.0f * 3.1415926f * (float)ii / (float)num_segments; //get the current angle

            float x = radius * cos(theta); //calculate the x component
            float y = radius * sin(theta); //calculate the y component

            glVertex3f(x + cx, y + cy, 0); //output vertex
        }
        glEnd();
    }
}

int draw_note(note n, uint64_t time)
{
    if (time > n.enter_time)
    {
        drawCircle(left + n.lane * tine_width + tine_width / 2, right - (1.0f * time - n.enter_time) * note_speed_multiplier, tine_width / 2, 30);
        return 1;
    }
    return 0;
}

void drawAxes()
{
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
}

void drawTines()
{
    glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    // printf("Starting at %.3f\n, with increment %.3f", right - left, (right - left) / N_TINES);
    for (float i = left; i < right - 0.1; i += tine_width)
    {
        // printf("Drawing line with x %.3f\n", i);
        glVertex3f(i, up, 0);
        glVertex3f(i, down, 0);
    }
    glEnd();
    printf("glEnd()\n");
}

void drawThreshold()
{
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(left, -0.8, 0);
    glVertex3f(right, -0.8, 0);
    glEnd();
}