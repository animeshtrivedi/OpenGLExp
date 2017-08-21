#include <stdio.h>
#include <GL/glut.h>

#include "digital_impressions/impression.h"

int main(int argc, char **argv) {
    printf("Hello, World to OpenGL experiments!\n");
    glutInit(&argc, argv);
    // this must be first
    //main_glpcview(argc, argv);
    init_xbox(argc, argv);
    start_impressions(argc, argv);
    glutMainLoop();
    return 0;
}
