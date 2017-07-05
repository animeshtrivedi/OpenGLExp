//
// Created by atr on 05.07.17.
//

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
    glColor3ub (211, 211, 211);

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

static void init (void) // this called once in the begenning
{
    glClearColor (1.0, 1.0, 1.0, 0.0);
#if 0
    glClear (GL_COLOR_BUFFER_BIT);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();

//    GLdouble left, GLdouble right,
//            GLdouble bottom, GLdouble top,
//            GLdouble near_val, GLdouble far_val)
//
    glOrtho (0.0, WIDTH, HEIGHT, 0.0, 0.0f, 1.0f);

    glMatrixMode (GL_MODELVIEW);
    //glLoadIdentity();

    //glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    glOrtho(0.0f, windowWidth, windowHeight, 0.0f, 0.0f, 1.0f);
#endif
}

static void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* drawing commands would go here, if we had any yet... */
    //drawgrid();

    glColor3f(0.0f, 0.0f, 0.0f);
    glRectf(-0.75f,0.75f, 0.75f, -0.75f);


    glutSwapBuffers();


//    glFlush();
//    glutSwapBuffers();
//    glutPostRedisplay();
}


void on_resize(int w, int h)
{
//    glViewport(0, 0, w, h);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
    printf(" Resize: w %d and h %d \n", w, h);
    curr_height = h;
    curr_width = w;
    //display(); // refresh window.
}


int exp_main (int argc, char **argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize (WIDTH, HEIGHT);
    glutCreateWindow ("Life Grid");

    init();

    glutDisplayFunc (display);
    //glutReshapeFunc(on_resize);

    //glDisable (GL_TEXTURE_2D);

    glutMainLoop();

    return 0;
}