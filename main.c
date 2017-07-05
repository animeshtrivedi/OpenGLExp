#include <stdio.h>
#include <GL/glut.h>
#include "all.h"

int main(int argc, char **argv) {
    printf("Hello, World to OpenGL experiments!\n");
#if 0
    /*
    source: https://en.wikibooks.org/wiki/OpenGL_Programming/Installation/GLUT
    */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("red 3D lighted cube");
    printf("GL_VERSION = %s\n",glGetString(GL_VERSION) ); */
    return 0;
#endif
    //triangle_main(argc, argv);
    //accanti_main(argc, argv);
    grid_main(argc, argv);
    return 0;
}
