#include <stdio.h>
#include <GL/glut.h>

int main(int argc, char **argv) {
    printf("Hello, World to OpenGL experiments!\n");
    init_xbox(argc, argv);
    start_impressions(argc, argv);
    glutMainLoop();
    return 0;
}
