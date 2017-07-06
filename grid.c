//
// Created by atr on 05.07.17.
//
#include <stdio.h>

#include <GL/freeglut.h>

#define WIDTH 1000
#define HEIGHT 400
#define OFFSET 200

static int curr_width = WIDTH;
static int curr_height = HEIGHT;

static void drawline (float x1, float y1, float x2, float y2)
{
    glBegin (GL_LINES);
    glVertex2f (x1, y1);
    glVertex2f (x2, y2);
    glEnd();
}

static void drawgrid()
{
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glColor3ub (255, 0, 0);

    for (float i = 0; i < curr_height; i += 10)
    {
        if ((int) i % 100 == 0) glLineWidth (3.0);
        else if ((int) i % 50 == 0) glLineWidth (2.0);
        else glLineWidth (1.0);
        drawline (0, i, (float) WIDTH, i);
    }

    for (float i = 0; i < curr_width; i += 10)
    {
        if ((int) i % 100 == 0) glLineWidth (3.0);
        else if ((int) i % 50 == 0) glLineWidth (2.0);
        else glLineWidth (1.0);
        drawline (i, 0, i, (float) HEIGHT);
    }
}

static void init (void)
{
    glClearColor (1.0, 1.0, 1.0, 0.0);

    glClear (GL_COLOR_BUFFER_BIT);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();

    glOrtho (0.0, WIDTH, HEIGHT, 0.0, 0.0f, 1.0f);

    glMatrixMode (GL_MODELVIEW);
}

static void display()
{
    drawgrid();
    glutSwapBuffers();
}

int grid_main (int argc, char **argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize (WIDTH, HEIGHT);
    glutCreateWindow ("Life Grid");

    init();

    glutDisplayFunc (display);

    glDisable (GL_TEXTURE_2D);

    glutMainLoop();

    return 0;
}

