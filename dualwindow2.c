//
// Created by atr on 06.07.17.
//

#include <GL/glut.h>

float clr = 0.2;

static void displayX()
{
    // clear the draw buffer .
    glClear(GL_COLOR_BUFFER_BIT);   // Erase everything

    // set the color to use in draw
    clr += 0.1;
    if ( clr>1.0)
    {
        clr=0;
    }
    // create a polygon ( define the vertexs)
    glBegin(GL_POLYGON); {
        glColor3f(clr, clr, clr);
        glVertex2f(-0.5, -0.5);
        glVertex2f(-0.5,  0.5);
        glVertex2f( 0.5,  0.5);
        glVertex2f( 0.5, -0.5);
    } glEnd();

    glFlush();
}

// Main execution  function
int dualwindow2_main(int argc, char *argv[])
{
    glutInit(&argc, argv);      // Initialize GLUT
    glutCreateWindow("win1");   // Create a window 1
    glutDisplayFunc(displayX);   // Register display callback
    glutCreateWindow("win2");   // Create a window 2
    glutDisplayFunc(displayX);   // Register display callback

    glutMainLoop();             // Enter main event loop
    return 0;
}
