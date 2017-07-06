//
// Created by atr on 06.07.17.
//

//
// Created by atr on 05.07.17.
//
#ifdef ATR_CYRIL

#include <stdio.h>
#include <malloc.h>
#include <GL/glut.h>
#include <string.h>

long global_time = 0;
struct square {
    /* x,y are lower left co-ordinates and then size s units */
    double x;
    double y;
    double s;
    /* colors */
    double r,g,b;
    /* cell index */
    int index;
    /* status */
    int isAlive;
};

//int init_state[] = {7, 12,17};
//int init_state[] = {9,10,15,16,19,20,25,26};
int init_state[] = {13,14,15,20,21,22};

/* current height and width of the window : initial 1024 x 768 */
int curr_height = 1024;
int curr_width = 768;

double bottom_left_x = -1.0;
double bottom_left_y = -1.0;

double top_right_x = 1.0;
double top_right_y = 1.0;

#define SIZEX 0.1

/* we start by 0.2 normalized square */
double sizeN = SIZEX;
double niceSlack = SIZEX/(10 * 2); // 10% of that

struct square *SX = NULL;
/* total number of squares */
int sq_total = 0;
/* total number on the x axis */
int sq_count_x = 0;
/* total number on the y axis */
int sq_count_y = 0;

int *next_generation_status;
/* calculates number of squares while keeping the area of the
 * square fixed. Hence when you resize, you will get more
 * squares. */
int calculate_rectangles_area() {
    return 0;
}

int init_alive(int index){
    int entries = sizeof(init_state) / sizeof(int);
    for(int i = 0 ; i < entries; i++){
        if (init_state[i] == index)
            return 1;
    }
    return 0;
}

/* calculates number of rectangles while keeping the number of
 * squares fixed. Hence when you resize, each squares gets
 * bigger or smaller */
int calculate_rectangles_num(){

    double xrange = top_right_x - bottom_left_x;
    double yrange = top_right_y - bottom_left_y;

    sq_count_x = xrange / sizeN;
    sq_count_y = yrange / sizeN;
    sq_total = sq_count_x * sq_count_y;
    printf("Number of fixed area squares are: %d \n", sq_total);
    /* we allocate them */
    if(SX){
        /* if it was not null then that means it is a recalculation */
        free(SX);
    }
    SX = (struct square *) calloc(sizeof(struct square), sq_total);
    next_generation_status = calloc(sizeof(int), sq_total);

    /* we start from bottom left size */
    double curr_x = bottom_left_x;
    double curr_y = bottom_left_y;

    for(int i = 0; i < sq_count_y; i++){
        for (int j = 0; j < sq_count_x; j++){
            struct square *xx = &SX[(i * sq_count_x) + j];
            xx->x = curr_x;
            xx->y = curr_y;
            xx->s = sizeN;
            xx->index = (i * sq_count_x) + j;
            xx->isAlive = init_alive(xx->index);
            printf(" Square number : %d , coordinates (%f, %f) \n", xx->index, xx->x, xx->y);
            curr_x+=sizeN;
        }
        /* move y one up */
        curr_y+=sizeN;
        /* reset x */
        curr_x = bottom_left_x;
    }
    return 0;
}

int walk_and_draw_rectangles(){

    struct square *s = SX;
    double x1, y1, x2, y2;
    for (int i = 0; i < sq_total; i++){
        x1 = s->x + niceSlack;
        y1 = s->y + niceSlack;
        x2 = s->x + sizeN - niceSlack;
        y2 = s->y + sizeN - niceSlack;

#if 0
        /* glBegin(GL_POLYGON);
		glVertex2(x1, y1);
		glVertex2(x2, y1);
		glVertex2(x2, y2);
		glVertex2(x1, y2);
		glEnd();
		*/

#else
        if(s->isAlive){
            glColor3f(0.2f, 0.2f, 0.2f);
        } else {
            glColor3f(0.9f,0.9f,0.9f);
        }
         glRectf(x1, y1, x2, y2);
#endif
        /* move to the next square */
        s++;
    }
}

/* set colors on squares */
void set_black(struct square *s){
    s->r = s->g = s->b= 0.0;
}

void set_white(struct square *s){
    s->r = s->g = s->b= 0.8;
}

int coordinates_to_arrindex(double x, double y){
    int indexX = (x - bottom_left_x) / sizeN;
    int indexY = (y - bottom_left_y) / sizeN;
    return ((indexY * sq_count_x) + indexX);
}

int is_outside(double x, double y){
    /* if you are drawing below , at the limit is fine */
    /* make sure that bottom left is inside the draw */
    int bottom_left = (x >= bottom_left_x) && (y >= bottom_left_y);
    int top_right = ((x + sizeN) <= top_right_x) && ( (y + sizeN) <= top_right_y);
    //printf(" bomttom left : %d top_right %d \n", bottom_left, top_right);
    return !(bottom_left && top_right);
}

int calculate_from_cordinate(double x, double y){
    if(is_outside(x,y)){
        //printf("\t declaring %f %f outside \n", x, y);
        return -1;
    }
    else {
        //printf("\t declaring %f %f inside \n", x, y);
        coordinates_to_arrindex(x,y);
    }
}

/* we calculate from the bottom left to top right neighbours */
int next_neighbour_index(struct square *s, int k){
    switch (k) {
        case 0: return calculate_from_cordinate(s->x - sizeN , s->y - sizeN);
        case 1: return calculate_from_cordinate(s->x         , s->y - sizeN);
        case 2: return calculate_from_cordinate(s->x + sizeN , s->y - sizeN);

        case 3: return calculate_from_cordinate(s->x - sizeN , s->y        );
        case 4: return -2; // this is you
        case 5: return calculate_from_cordinate(s->x + sizeN , s->y        );

        case 6: return calculate_from_cordinate(s->x - sizeN , s->y + sizeN);
        case 7: return calculate_from_cordinate(s->x         , s->y + sizeN);
        case 8: return calculate_from_cordinate(s->x + sizeN , s->y + sizeN);
    }
}

int calculate_next_generation(struct square *s, int index)
{
    int neix[9], total_alive = 0, total_dead = 0, stx;
    for(int i= 0 ; i < 9; i++){
        neix[i] = next_neighbour_index(s, i);
    }
    printf(" Index %d has neighbours as : ", s->index);
    for(int i= 0 ; i < 9; i++){
        printf(" : %d ", neix[i]);
        /* for all the valid entries */
        if(neix[i] >= 0){
            if(SX[neix[i]].isAlive){
                total_alive++;
            } else {
                total_dead++;
            }
        }
    }
    printf(" || total_alive %d , total_dead: %d alive? %d ", total_alive, total_dead, s->isAlive);
    /* for this square the next state is */
    if(s->isAlive){
        /* if this was alive */
        if(total_alive < 2){
            next_generation_status[index] = 0; // dead, underpopulation
            printf(" result: dead (underpopulation) \n");
        } else if (total_alive >= 2 && total_alive <= 3){
            next_generation_status[index] = 1; // ok
            printf(" result: survive ! (OK) \n");
        } else if (total_alive > 3){
            next_generation_status[index] = 0; // ok, over-population
            printf(" result: dead (over population) \n");
        } else {
            printf(" ### we stay alive? total_alive %d total_dead %d \n", total_alive, total_dead);
        }
    } else {
        /* we were dead */
        if(total_alive >= 3){
            next_generation_status[index] = 1; // re-surrected
            printf(" result: alive (resurrected) \n");
        } else {
            printf(" ### we stay dead? total_alive %d total_dead %d \n", total_alive, total_dead);
        }
    }
    return 0;
}

int run_scan(){
    /* clean next generation */
    bzero(next_generation_status, sizeof(int) * sq_total);

    /* calculate next generation status */
    for(int i =0; i < sq_total; i++){
        calculate_next_generation(SX + i, i);
    }
    /* apply next generation */
    for(int i =0; i < sq_total; i++){
        SX[i].isAlive = next_generation_status[i];
    }
}


static void init (void)
{
    calculate_rectangles_num();

    glClearColor (1.0, 1.0, 1.0, 0.0);
//
//    glClear (GL_COLOR_BUFFER_BIT);
//    glMatrixMode (GL_PROJECTION);
//    glLoadIdentity();
//
//    glOrtho (0.0, WIDTH, HEIGHT, 0.0, 0.0f, 1.0f);
//
//    glMatrixMode (GL_MODELVIEW);
}

static void display()
{
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    walk_and_draw_rectangles();
    glutSwapBuffers();
}

static void timer_redraw(int value){
    printf(" Execution for scale %ld \n", global_time);
    run_scan();
    //something
    glutPostRedisplay();
    // 1000 milliseconds
    global_time++;
    glutTimerFunc(1000, timer_redraw, 0);
}

int cyril_main (int argc, char **argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow ("Life Grid Example");

    init();

    glutDisplayFunc (display);

    glDisable (GL_TEXTURE_2D);

    glutTimerFunc(1000, timer_redraw, 0);
    glutMainLoop();

    return 0;
}
#endif