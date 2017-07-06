//
// Created by atr on 06.07.17.
//

#include <GL/glut.h>
#include <stdio.h>

float clr1 = 0.2;
float clr2 = 0.6;

static void displayX1()
{
    // clear the draw buffer .
    glClear(GL_COLOR_BUFFER_BIT);   // Erase everything

    // set the color to use in draw
    clr1 += 0.1;
    if ( clr1 > 1.0)
    {
        clr1 = 0;
    }
    // create a polygon ( define the vertexs)
    glBegin(GL_POLYGON); {
        glColor3f(clr1, clr1, clr1);
        glVertex2f(-0.5, -0.5);
        glVertex2f(-0.5,  0.5);
        glVertex2f( 0.5,  0.5);
        glVertex2f( 0.5, -0.5);
    } glEnd();

    glFlush();
}

static void displayX2()
{
    // clear the draw buffer .
    glClear(GL_COLOR_BUFFER_BIT);   // Erase everything

    // set the color to use in draw
    clr2 += 0.1;
    if ( clr2 > 1.0)
    {
        clr2 = 0;
    }
    // create a polygon ( define the vertexs)
    glBegin(GL_POLYGON); {
        glColor3f(clr2, clr2, clr2);
        glVertex2f(-0.5, -0.5);
        glVertex2f(-0.5,  0.5);
        glVertex2f( 0.5,  0.5);
        glVertex2f( 0.5, -0.5);
    } glEnd();

    glFlush();
}

static void timer_redraw1(int value){
    printf(" Execution1111 for 1000s \n");
    // do something
    glutPostRedisplay();
    // 1000 milliseconds
    glutTimerFunc(1000, timer_redraw1, 0);
}

static void timer_redraw2(int value){
    printf(" Execution5555 for 500ms \n");
    // do something
    glutPostRedisplay();
    // 1000 milliseconds
    glutTimerFunc(500, timer_redraw2, 0);
}

// Main execution  function
int dualwindow2_main(int argc, char *argv[])
{
    glutInit(&argc, argv);      // Initialize GLUT

    glutCreateWindow("win1");   // Create a window 1
    glutDisplayFunc(displayX1);   // Register display callback
    glutTimerFunc(1000, timer_redraw1, 0);

    glutCreateWindow("win2");   // Create a window 2
    glutDisplayFunc(displayX2);   // Register display callback
    glutTimerFunc(500, timer_redraw2, 0);

    glutMainLoop();             // Enter main event loop
    return 0;
}
