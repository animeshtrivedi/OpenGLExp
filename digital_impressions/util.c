//
// Created by atr on 31.07.17.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "impression.h"

void init_window_state(struct window_state *wstate,
		double bottom_left_x,
			double bottom_left_y,
			double top_right_x,
			double top_right_y,
			double size,
			double slack){

	wstate->slack = slack;
	wstate->bottom_left_x = bottom_left_x;
	wstate->bottom_left_y = bottom_left_y;
	wstate->top_right_x = top_right_x;
	wstate->top_right_y = top_right_y;
	wstate->size = 0.1;

    wstate->max_xrange = top_right_x - bottom_left_x;
    wstate->max_yrange = top_right_y - bottom_left_y;
    wstate->sq_count_x = wstate->max_xrange / size;
    wstate->sq_count_y = wstate->max_yrange / size;
    wstate->sq_total = wstate->sq_count_x * wstate->sq_count_y;

    printf("init: number of squares are : %d  | xrange: %f yrange: %f sizeN: %f \n",
    		wstate->sq_total, wstate->max_xrange, wstate->max_yrange, wstate->size);
}

void init_default_new_size(struct window_state *state, double size){
	init_window_state(state, -1.0, -1.0, 1.0, 1.0, size, 0);
}

void setup_gol_state(struct gol_state *gstate, int items) {

}

void setup_squares(struct global_win1 *gwin){
	struct square *SX = gwin->squares;
    if(SX){
        /* if it was not null then that means it is a recalculation */
        free(SX);
    }
    SX = (struct square *) calloc(sizeof(struct square), gwin->wstate.sq_total);
    /* we start from bottom left side */
    double curr_x = gwin->wstate.bottom_left_x;
    double curr_y = gwin->wstate.bottom_left_y;

    for(int i = 0; i < gwin->wstate.sq_count_y; i++){
        for (int j = 0; j < gwin->wstate.sq_count_x; j++){
            struct square *xx = &SX[(i * gwin->wstate.sq_count_x) + j];
            xx->x = curr_x;
            xx->y = curr_y;
            xx->s = gwin->wstate.size;
            xx->index = (i * gwin->wstate.sq_count_x) + j;
            curr_x+=gwin->wstate.size;
        }
        /* move y one up */
        curr_y+=gwin->wstate.size;
        /* reset x */
        curr_x = gwin->wstate.bottom_left_x;
    }
}


void walk_and_draw_color(struct global_win1 *gwin, int verbose) {
    struct square *sx = gwin->squares;
    struct color_state *cols = gwin->cstate;
    double x1, y1, x2, y2;
    for (int i = 0; i < gwin->wstate.sq_total; i++){
        x1 = sx[i].x + gwin->wstate.slack;
        y1 = sx[i].y + gwin->wstate.slack;
        x2 = sx[i].x + gwin->wstate.size - gwin->wstate.slack;
        y2 = sx[i].y + gwin->wstate.size - gwin->wstate.slack;
        glColor3f(cols[i].r, cols[i].g, cols[i].b);
        glRectf(x1, y1, x2, y2);
        if(verbose){
        	glPrintSquare(&sx[i], &cols[i]);
        }
    }
}

void glPrintText(double x, double y, double z, char *string)
{
	glColor3f(0, 0, 0);
	glRasterPos2f(x, y);
	int len = (int) strlen(string);
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
}

void glPrintSquare(struct square *s, struct color_state *cstate)
{
	char *newx = calloc(1024, 1);
	char *X;
	if(cstate->r > cstate->g){
		if(cstate->r > cstate->b)
			X="RED";
		else
			X="BLUE";
	} else {
		if(cstate->g > cstate->b)
			X="GREEN";
		else
			X="BLUE";
	}

	snprintf(newx, 32, "(%0.2f, %0.2f, %0.2f) %s",
			cstate->r,
			cstate->g,
			cstate->b, X);
	glPrintText(s->x, s->y, 0.0, newx);
}

/* set colors on squares */
void set_black(struct color_state *cstate){
	set_color(cstate, 0.0, 0.0, 0.0);
}

void set_white(struct color_state *cstate){
	set_color(cstate, 0.9, 0.9, 0.9);
}

void set_color(struct color_state *cstate, double r, double g, double b){
	/* what we do here is to swap the color generations */
	cstate->last_r = cstate->r;
	cstate->last_g = cstate->g;
	cstate->last_b = cstate->b;
	/* now assign the current generation */
	cstate->r = r;
	cstate->g = g;
	cstate->b = b;
}

int xy_to_arrindex(struct window_state *wstate, double x, double y){
    int indexX = (x - wstate->bottom_left_x) / wstate->size;
    int indexY = (y - wstate->bottom_left_y) / wstate->size;
    return ((indexY * wstate->sq_count_x) + indexX);
}

int is_xy_outside(struct window_state *wstate, double x, double y){
    /* if you are drawing below , at the limit is fine */
    /* make sure that bottom left is inside the draw */
    int bottom_left = (x >= wstate->bottom_left_x) &&
    		(y >= wstate->bottom_left_y);
    int top_right = ((x + wstate->size) <= wstate->top_right_x) &&
    		((y + wstate->size) <= wstate->top_right_y);
    return !(bottom_left && top_right);
}


int get_valid_index_if_inside(struct window_state *wstate, double x, double y){
    if(is_xy_outside(wstate, x, y)){
        return -1;
    }
    else {
        xy_to_arrindex(wstate, x, y);
    }
}

/* we calculate from the bottom left to top right neighbours */
int next_neighbour_index(struct square *s,
		struct window_state *wstate,
		double size,
		int k){
    switch (k) {
        case 0: return get_valid_index_if_inside(wstate, s->x - size , s->y - size);
        case 1: return get_valid_index_if_inside(wstate, s->x         , s->y - size);
        case 2: return get_valid_index_if_inside(wstate, s->x + size , s->y - size);

        case 3: return get_valid_index_if_inside(wstate, s->x - size , s->y        );
        case 4: return -2; // this is you
        case 5: return get_valid_index_if_inside(wstate, s->x + size , s->y        );

        case 6: return get_valid_index_if_inside(wstate, s->x - size , s->y + size);
        case 7: return get_valid_index_if_inside(wstate, s->x         , s->y + size);
        case 8: return get_valid_index_if_inside(wstate, s->x + size , s->y + size);
    }
}


void calculate_movement_from_squares(struct global_win1 *gwin){
	/* at this point we have previous generation and the next generation
	 * color data - we can calculate our global average of things
	 */
}

void calculate_avg_color_from_xboxdata(struct global_win1 *gwin, int sq_number,
		uint8_t *xbox_data) {
	/* s is the entry in the global array */
	/* we need to find x and y ranges in 640 x 480 */

	double xMultiplier = XBOX_WIDTH / gwin->wstate.max_xrange;
	double yMultiplier = XBOX_HEIGHT / gwin->wstate.max_yrange;
	struct square *s = &gwin->squares[sq_number];

	int x_start = (s->x - gwin->wstate.bottom_left_x) * xMultiplier;
	int x_end = (s->x + s->s - gwin->wstate.bottom_left_x) * xMultiplier;

	int y_start = (s->y - gwin->wstate.bottom_left_y) * yMultiplier;
	int y_end = (s->y + s->s - gwin->wstate.bottom_left_y) * yMultiplier;

	assert(x_start >= 0 && x_start <= XBOX_WIDTH);
	assert(x_end >= 0 && x_end <= XBOX_WIDTH);

	assert(y_start >= 0 && y_start <= XBOX_HEIGHT);
	assert(y_end >= 0 && y_end <= XBOX_HEIGHT);

	double rAvg = 0 , gAvg = 0 , bAvg = 0;

	int ylimit = y_end - y_start;
	int xlimit = x_end - y_end;
	int items = XBOX_HEIGHT * XBOX_WIDTH;
	for(int i = x_start; i <= x_end; i++){
		for(int j = y_start; j <= y_end; j++){
			// Flat[x + WIDTH * (y + DEPTH * z)] = Original[x, y, z]
			// https://stackoverflow.com/questions/7367770/how-to-flatten-or-index-3d-array-in-1d-array
	        int index = ((XBOX_HEIGHT - j) * XBOX_WIDTH * 3) + (i * 3);
	        rAvg+=xbox_data[index + 0];
	        gAvg+=xbox_data[index + 1];
	        bAvg+=xbox_data[index + 2];
		}
	}
	// if the value is byte then to normalize it we must divide by 255
	double xxx = ((x_end - x_start) * (y_end - y_start) * 255);
	rAvg/=xxx;
	gAvg/=xxx;
	bAvg/=xxx;
	set_color(&gwin->cstate[sq_number], rAvg, gAvg, bAvg);
}


void calculate_next_generation_wikialgo(struct global_win1 *gwin, int index)
{
    int neighbours[9], total_alive = 0, total_dead = 0;
    struct square *s = &gwin->squares[index];
    struct gol_state *gstate = gwin->gstate;
    for(int i= 0 ; i < 9; i++){
        neighbours[i] = next_neighbour_index(gwin->squares,
        		&gwin->wstate,
        		gwin->wstate.size,
        		i);
    }
    for(int i= 0 ; i < 9; i++){
        /* for all the valid entries */
        if(neighbours[i] >= 0){
            if(gstate[neighbours[i]].isAlive){
                total_alive++;
            } else {
                total_dead++;
            }
        }
    }
    /* for this square the next state is */
    if(gstate[index].isAlive){
        /* if this was alive */
        if(total_alive < 2){
        	gstate[index].nx_isAlive = 0; // dead, underpopulation
        } else if (total_alive >= 2 && total_alive <= 3){
        	gstate[index].nx_isAlive = 1; // ok
        } else if (total_alive > 3){
        	gstate[index].nx_isAlive = 0; // ok, over-population
        } else {
            //printf(" ### we stay alive? total_alive %d total_dead %d \n", total_alive, total_dead);
        }
    } else {
        /* we were dead */
        if(total_alive == 3){
        	gstate[index].nx_isAlive = 1; // re-surrected
        } else {
        }
    }
}

void assign_next_generation_wikialgo(struct gol_state *gstate, int items){
	for(int i =0; i < items; i++)
		gstate[i].isAlive = gstate[i].nx_isAlive;
}

/* it is the responsibility of the caller to set the right window */
int run_scan_for_wikialgo(struct global_win1 *win){
    /* clean next generation */
	int items = win->wstate.sq_total;
	for(int i = 0 ;i < items; i++){
		win->gstate[i].nx_isAlive = 0;
	}
	/* calculate the next generation */
	for(int i = 0 ;i < items; i++){
		calculate_next_generation_wikialgo(win, i);
	}
	/* now we swap */
	assign_next_generation_wikialgo(win->gstate, items);
	/* now we need to draw - assign colors first */
	for(int i = 0 ;i < items; i++){
		if(win->gstate[i].isAlive)
			set_white(&win->cstate[i]);
		else
			set_black(&win->cstate[i]);
	}
	/* now we draw */
	walk_and_draw_color(win, 0);
	return 0;
}
