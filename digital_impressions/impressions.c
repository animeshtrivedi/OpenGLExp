#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "impression.h"

static struct global_win1 *windows;

// works! -- second example
//int init_state[] = {13,14,15,20,21,22};
//#define SIZEX 0.3

// example 3 - works
//static int init_state[] = {9,10,15,16,19,20,25,26};
//#define SIZEX 0.3

/////////////////////////////////////////////////
static void _timer_all(struct global_win1 *gwin,
		void (*funx)(struct global_win1 *gwin),
		void (* callback)( int ), int value){
    glutSetWindow(gwin->window_number);
    funx(gwin);
    glutPostRedisplay();
    glutTimerFunc(gwin->timeout, callback, value);
}

static void __display_all(struct global_win1 *gwin, int verbose)
{
    glutSetWindow(gwin->window_number);
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    walk_and_draw_color(gwin, verbose);
    glutSwapBuffers();
}

static void _display_all(struct global_win1 *gwin){
	__display_all(gwin, 0);
}

static void _setup_window_gol(struct global_win1 *gwin,
		double size,
		char *win_name,
		void (* display_cb)( void ),
		unsigned int timeout,
		void (* timeout_cb)( int ), int timeout_cb_value,
		int *init_arr,
		int init_arr_sz)
{
	gwin->width = 640;
	gwin->height = 480;
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(gwin->width, gwin->height);
	glutInitWindowPosition(0, 550);
	gwin->name = win_name;
	gwin->window_number = glutCreateWindow (gwin->name);
	init_default_new_size(&gwin->wstate, size);
	setup_squares(gwin);
	setup_gol_state(&gwin->gstate, gwin->wstate.sq_total);
	for(int i = 0 ; i < init_arr_sz; i++){
		gwin->gstate[init_arr[i]].isAlive = 1;
	}
	assign_gol_colors(gwin->gstate, gwin->cstate, gwin->wstate.sq_total);
	gwin->timeout = timeout; // 1000 msec
	glutDisplayFunc(display_cb);
	glutTimerFunc(gwin->timeout, timeout_cb, timeout_cb_value);
}

/////////////////////////////////////////////////
static void timer_redraw_0(int value){
	struct global_win1 *gwin =  &windows[value];
	_timer_all(gwin, run_scan_for_wikialgo, timer_redraw_0, value);
}

static void display_0()
{
	_display_all(&windows[0]);
}

static void setup_window_gol0(int gindex){
	struct global_win1 *gwin = &windows[gindex];
	int init_state[] = {7, 12, 17};
	double size = 0.4;
	_setup_window_gol(gwin,
			size,
			"gol -window #0",
			display_0,
			1000,
			timer_redraw_0,
			gindex,
			init_state,
			sizeof(init_state) / sizeof(int));
}

///////////////// second window
static void timer_redraw_1(int value){
	struct global_win1 *gwin =  &windows[value];
	_timer_all(gwin, run_scan_for_wikialgo, timer_redraw_1, value);
}

static void display_1()
{
	_display_all(&windows[1]);
}

static void setup_window_gol1(int gindex){
	struct global_win1 *gwin = &windows[gindex];
	int init_state[] = {13,14,15,20,21,22};
	double size = 0.33;
	_setup_window_gol(gwin,
			size,
			"gol -window #1",
			display_1,
			1000,
			timer_redraw_1,
			gindex,
			init_state,
			sizeof(init_state) / sizeof(int));
}

///////////////// third window
static void timer_redraw_2(int value){
	struct global_win1 *gwin =  &windows[value];
	_timer_all(gwin, run_scan_for_wikialgo, timer_redraw_2, value);
}

static void display_2()
{
	_display_all(&windows[2]);
}

static void setup_window_gol2(int gindex){
	struct global_win1 *gwin = &windows[gindex];
	int init_state[] = {9,10,15,16,19,20,25,26};
	double size = 0.33;
	_setup_window_gol(gwin,
			size,
			"gol -window #2",
			display_2,
			1000,
			timer_redraw_2,
			gindex,
			init_state,
			sizeof(init_state) / sizeof(int));
}

///////////////////////////// now these are xbox functions

static void timer_box_3(int value){
	struct global_win1 *gwin =  &windows[value];
	_timer_all(gwin,
			run_scan_for_xboxdata_depth,
			timer_box_3,
			value);
}

static void display_3()
{
	__display_all(&windows[3], 1);
}

static void setup_window_xbox3(int gindex){
	struct global_win1 *gwin = &windows[gindex];

	gwin->width = 640;
	gwin->height = 480;
	glutInitWindowSize(gwin->width, gwin->height);
	glutInitWindowPosition(0, 550);
	gwin->name = "xbox + square colors";
	gwin->window_number = glutCreateWindow (gwin->name);
	init_default_new_size(&gwin->wstate, 0.25);
	setup_squares(gwin);
	run_scan_for_xboxdata_depth(gwin);
	gwin->timeout = 1000; // 1000 msec
	glutDisplayFunc(display_3);
	glutTimerFunc(gwin->timeout, timer_box_3, gindex);
}

///////////////////////////// now these are xbox functions

static void allocate_state(struct global_win1 *gwin, double size) {
	init_default_new_size(&gwin->wstate, size);
	setup_squares(gwin);
}

static void timer_box_zoom(int value){
	static long countx = 0;
	struct global_win1 *gwin =  &windows[value];
	countx++;
	double size = calculate_zoom_size(gwin, countx);
	allocate_state(gwin, size);
	// between 1,000 and 10,000 milliseconds
	int new_random_timeout = get_next_int(1000, 5000);
	gwin->timeout = new_random_timeout;
	_timer_all(gwin,
			run_scan_for_xboxdata_depth,
			timer_box_zoom,
			value);
}

static void display_zoom()
{
	_display_all(&windows[4]);
}

static void setup_window_xbox_zoom(int gindex){
	struct global_win1 *gwin = &windows[gindex];

	gwin->width = 640;
	gwin->height = 480;
	glutInitWindowSize(gwin->width, gwin->height);
	glutInitWindowPosition(0, 550);
	gwin->name = "xbox + zoom";
	gwin->window_number = glutCreateWindow (gwin->name);
	allocate_state(gwin, 0.1);

	gwin->timeout = 1000; // 1000 msec
	glutDisplayFunc(display_zoom);
	glutTimerFunc(gwin->timeout, timer_box_zoom, gindex);
}

///////////////////////////// now these are xbox + GOL functions

static void display_xbox_gol()
{
	_display_all(&windows[5]);
}

static void timer_xbox_gol(int value){
	/* now in this function we need to run the GOL and then assign color */
	struct global_win1 *gwin =  &windows[value];
	gwin->gstate->generation_number++;

	// every 10th generation we refresh
	if(gwin->gstate->generation_number % 10 == 0){
		run_scan_for_xboxdata_depth(gwin);
		for(int i = 0 ; i < gwin->wstate.sq_total; i++){
			if(rand() % 2 == 0)
				gwin->gstate[i].isAlive = 1;
			else
				gwin->gstate[i].isAlive = 0;
		}
	}
	_timer_all(gwin,
			run_scan_for_wikialgo_xbox,
			timer_xbox_gol,
			value);
}

static void setup_window_xbox_gol(int gindex){
	struct global_win1 *gwin = &windows[gindex];

	/* this is the size and number of squares */
	double size = 0.1;
	gwin->width = 640;
	gwin->height = 480;
	glutInitWindowSize(gwin->width, gwin->height);
	glutInitWindowPosition(0, 550);
	gwin->name = "xbox + GOL";
	gwin->window_number = glutCreateWindow (gwin->name);
	init_window_state(&gwin->wstate, -1.0, -1.0, 1.0, 1.0, size, 0.01);
	setup_squares(gwin);
	setup_gol_state(&gwin->gstate, gwin->wstate.sq_total);
	gwin->gstate->generation_number = 0;

	//https://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c
	srand(time(NULL));   // should only be called once

	// this needs to be randomized
	for(int i = 0 ; i < gwin->wstate.sq_total; i++){
		if(rand() % 2 == 0)
			gwin->gstate[i].isAlive = 1;
		else
			gwin->gstate[i].isAlive = 0;
	}

	// This needs to change, you get colors from your average calculation
	run_scan_for_xboxdata_depth(gwin);

	gwin->timeout = 1000; // 1000 msec
	glutDisplayFunc(display_xbox_gol);
	glutTimerFunc(gwin->timeout, timer_xbox_gol, gindex);
}

/////////////////////////////////////////////////////////

static void display_cyclic()
{
	_display_all(&windows[6]);
}

static void timer_cyclic(int value){
	/* now in this function we need to run the GOL and then assign color */
	struct global_win1 *gwin =  &windows[value];
	gwin->gstate->generation_number++;
	printf(" generation %lu \n", gwin->gstate->generation_number);
	if(gwin->gstate->generation_number == 5 || gwin->gstate->generation_number%100 == 0) {
		gwin->timeout = 300;
#if 0
		run_scan_for_xboxdata_depth(gwin);
#else
		for(int i = 0; i < gwin->wstate.sq_total; i++){
			gwin->cstate[i].r = get_next_double();
			gwin->cstate[i].g = get_next_double();
			gwin->cstate[i].b = get_next_double();
			set_color(&gwin->cstate[i], gwin->cstate[i].r, gwin->cstate[i].g, gwin->cstate[i].b);
			//printf(" got %d - %f %f %f \n", i, gwin->cstate[i].r, gwin->cstate[i].g, gwin->cstate[i].b);
		}
#endif
	}

	_timer_all(gwin,
			run_scan_for_cyclic_automaton,
			timer_cyclic,
			value);
}

static void setup_window_xbox_cyclic_automaton(int gindex){
	struct global_win1 *gwin = &windows[gindex];
	/* this is the size and number of squares */
	double size = 0.01;
	gwin->width = 640;
	gwin->height = 480;
	glutInitWindowSize(gwin->width, gwin->height);
	glutInitWindowPosition(0, 550);
	gwin->name = "cyclic";
	gwin->window_number = glutCreateWindow (gwin->name);
	init_window_state(&gwin->wstate, -1.0, -1.0, 1.0, 1.0, size, 0);
	setup_squares(gwin);
	setup_gol_state(&gwin->gstate, gwin->wstate.sq_total);
	gwin->gstate->generation_number = 0;

	gwin->timeout = 1000; // 1000 msec
	glutDisplayFunc(display_cyclic);
	glutTimerFunc(gwin->timeout, timer_cyclic, gindex);
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
	windows = calloc(sizeof(*windows), 7);
	setup_window_gol0(0);
	setup_window_gol1(1);
	setup_window_gol2(2);
	/* This one shows the colors in a square */
	setup_window_xbox3(3);
	/* this one does variable zoom */
	setup_window_xbox_zoom(4);
	/* this one does xbox + GOL */
	setup_window_xbox_gol(5);
	/* TODO: https://en.wikipedia.org/wiki/Cyclic_cellular_automaton */
	setup_window_xbox_cyclic_automaton(6);
}
