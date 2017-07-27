//
// Created by atr on 05.07.17.
//

#include <GL/gl.h>
#include <GL/glut.h>


//http://math.hws.edu/bridgeman/courses/324/s06/doc/opengl.html
static void display () {

    /* clear window */
    glClear(GL_COLOR_BUFFER_BIT);

    /* draw scene */
    //glutSolidTeapot(.5);
    glutWireTeapot(.5);

    /* flush drawing routines to the window */
    glFlush();

}

int teapot_main ( int argc, char * argv[] ) {

    /* initialize GLUT, using any commandline parameters passed to the
       program */
    glutInit(&argc,argv);

    /* setup the size, position, and display mode for new windows */
    glutInitWindowSize(500,500);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode(GLUT_RGB);

    /* create and set up a window */
    glutCreateWindow("hello, teapot!");
    glutDisplayFunc(display);

    /* tell GLUT to wait for events */
    glutMainLoop();
}


