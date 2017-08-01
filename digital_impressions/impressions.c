#include "impression.h"

static struct global_win1 *windows;

// works! -- second example
//int init_state[] = {13,14,15,20,21,22};
//#define SIZEX 0.3

// example 3 - works
//static int init_state[] = {9,10,15,16,19,20,25,26};
//#define SIZEX 0.3


static void timer_redraw_0(int value){
	struct global_win1 *gwin =  &windows[value];
    glutSetWindow(gwin->window_number);
    run_scan_for_wikialgo(&windows[value]);
    assign_gol_colors(gwin->gstate,
    		gwin->cstate,
    		gwin->wstate.sq_total);
    glutPostRedisplay();
    glutTimerFunc(1000, timer_redraw_0, 0);
}

static void display_0()
{
    glutSetWindow(windows[0].window_number);
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    /* now we draw */
    walk_and_draw_color(&windows[0], 0);
    glutSwapBuffers();
}

static void setup_window_0_gol0(struct global_win1 *gwin, int index){
	int init_state[] = {7, 12, 17};
	double size = 0.4;
	gwin->width = 640;
	gwin->height = 480;
	glutInitWindowSize(gwin->width, gwin->height);
	glutInitWindowPosition(0, 550);
	gwin->window_number = glutCreateWindow ("gol - wiki #1");
	init_default_new_size(&gwin->wstate, size);
	setup_squares(gwin);
	setup_gol_state(&gwin->gstate, gwin->wstate.sq_total);
	gwin->cstate = calloc(sizeof(*gwin->cstate), gwin->wstate.sq_total);
	int entries = sizeof(init_state) / sizeof(int);
	for(int i = 0 ; i < entries; i++){
		gwin->gstate[init_state[i]].isAlive = 1;
	}
	assign_gol_colors(gwin->gstate, gwin->cstate, gwin->wstate.sq_total);

	glutDisplayFunc(&display_0);
	glutTimerFunc(1000, timer_redraw_0, index);
}

void start_impressions(){
	/* we need to setup the xbox window */
	/*
	 * 1. just the basic algo version from the wikipedia 3x = 0 , 1, 2
	 * 2. a average color window with text, lets say with 16 squares = 3
	 * 3. the zoom-in and out window = 4
	 * 4. GOL with xbox data logic = 5
	 * 5. something with distance and refresh rate = 6
	 */
	windows = calloc(sizeof(*windows), 1);
	setup_window_0_gol0(&windows[0], 0);
	printf(" ------------ done ");
}
