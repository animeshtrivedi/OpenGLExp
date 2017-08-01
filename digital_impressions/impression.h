#ifndef IMPRESSION_H
#define IMPRESSION_H

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#define XBOX_HEIGHT 480
#define XBOX_WIDTH 640

struct square {
    /* x,y are lower left co-ordinates and then size s units */
    double x;
    double y;
    double s;
    /* cell index */
    int index;
};

struct color_state{
    /* colors */
    double r,g,b;
    /* last generation */
    double last_r, last_g, last_b;
};

struct gol_state{
	/* current generation status */
	int isAlive;
	/* next generation status */
	int nx_isAlive;
};

struct window_state{
	double bottom_left_x;
	double bottom_left_y;
	double top_right_x;
	double top_right_y;
	double size;
	/* derivative values */
    double max_xrange;
    double max_yrange;
    int sq_count_x;
    int sq_count_y;
    int sq_total;
    /* slack for nice printing */
    double slack;
};

struct global_gol{
	/* this contains squares and the gol_state with some global
	 * variables
	 */
	struct square *square;
	struct gol_state *gol_state;
};

struct global_win1{
	/* this contains squares and the gol_state with some global
	 * variables
	 */
	struct square *squares;
	struct color_state *cstate;
	struct gol_state *gstate;

	struct window_state wstate;
	int width;
	int height;
	int window_number;
	char *name;
};

extern void init_window_state(struct window_state *state,
		double bottom_left_x,
			double bottom_left_y,
			double top_right_x,
			double top_right_y,
			double size,
			double slack);

extern void init_default_new_size(struct window_state *state, double size);
extern void setup_gol_state(struct gol_state **gstate, int items);
extern void setup_squares(struct global_win1 *gwin);
extern void walk_and_draw_color(struct global_win1 *gwin, int verbose);
extern void glPrintText(double x, double y, double z, char *string);
extern void glPrintSquare(struct square *s, struct color_state *cstate);
extern void set_black(struct color_state *cstate);
extern void set_white(struct color_state *cstate);
extern void set_color(struct color_state *cstate, double r, double g, double b);
extern int run_scan_for_wikialgo(struct global_win1 *win);
extern void walk_and_draw_color(struct global_win1 *gwin, int verbose);
extern int init_xbox(int argc, char **argv);
extern void start_impressions();
extern void assign_gol_colors(struct gol_state *gstate, struct color_state *cstate, int items);
#endif
