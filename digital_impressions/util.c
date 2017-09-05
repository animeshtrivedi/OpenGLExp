//
// Created by atr on 31.07.17.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

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
	wstate->size = size;

    wstate->max_xrange = top_right_x - bottom_left_x;
    wstate->max_yrange = top_right_y - bottom_left_y;
    wstate->sq_count_x = wstate->max_xrange / size;
    wstate->sq_count_y = wstate->max_yrange / size;
    wstate->sq_total = wstate->sq_count_x * wstate->sq_count_y;

//    printf("init: number of squares are : %d  | xrange: %f yrange: %f sizeN: %f \n",
//    		wstate->sq_total, wstate->max_xrange, wstate->max_yrange, wstate->size);
}

void init_default_new_size(struct window_state *state, double size){
	init_window_state(state, -1.0, -1.0, 1.0, 1.0, size, 0.01);
}

void setup_gol_state(struct gol_state **gstate, int items) {
	*gstate = calloc(sizeof(**gstate), items);
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

//    printf("Number of fixed area squares are: %d  | xrange: %f yrange: %f sizeN: %f \n",
//        		gwin->wstate.sq_total, gwin->wstate.max_xrange,
//        		gwin->wstate.max_yrange, gwin->wstate.size);

    for(int i = 0; i < gwin->wstate.sq_count_y; i++){
        for (int j = 0; j < gwin->wstate.sq_count_x; j++){
            struct square *xx = &SX[(i * gwin->wstate.sq_count_x) + j];
            xx->x = curr_x;
            xx->y = curr_y;
            xx->s = gwin->wstate.size;
            xx->index_x = j;
            xx->index_y = i;
            xx->index = (i * gwin->wstate.sq_count_x) + j;
//            printf(" \t init SQ %d at %f,%f\n", xx->index, xx->x, xx->y);
            curr_x+=gwin->wstate.size;
        }
        /* move y one up */
        curr_y+=gwin->wstate.size;
        /* reset x */
        curr_x = gwin->wstate.bottom_left_x;
    }
    gwin->squares = SX;
    // allocate squares as well
    gwin->cstate = calloc(sizeof(*gwin->cstate), gwin->wstate.sq_total);
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
	glRasterPos2f(x + 0.01, y + 0.1); // here is the slack
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

	snprintf(newx, 32, "(%0.2f,%0.2f,%0.2f) %s",
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
	if(r > 1.0)
		r = 1.0;
	if(g > 1.0)
			g = 1.0;
	if(b > 1.0)
			b = 1.0;

	cstate->r = r;
	cstate->g = g;
	cstate->b = b;
	cstate->colx = calculate_cyclic_number(r, g, b, 3);
}

#if 0
int xy_to_arrindex(struct window_state *wstate, double x, double y){
    double indexXk = (x - wstate->bottom_left_x) / wstate->size;
    double indexYk = (y - wstate->bottom_left_y) / wstate->size;
    int indexX = ceil(indexXk);
    int indexY = ceil(indexYk);
    int index =  ((indexY * wstate->sq_count_x) + indexX);
    printf(" calculating %f %f (%f %f) to index %d || %d and %d \n", x, y,
    		ceil(indexXk), ceil(indexYk),
    		index, indexX, indexY);
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

#else
int is_xy_outside(struct window_state *wstate, int x, int y){
    int x_in = (x >= 0 && x < wstate->sq_count_x);
    int y_in = (y >= 0 && y < wstate->sq_count_y);
    //printf(" %d %d is outside?: %d \n", x, y, !(x_in && y_in));
    return !(x_in && y_in);
}


int get_valid_index_if_inside(struct window_state *wstate, int x, int y){
    if(is_xy_outside(wstate, x, y)){
        return -1;
    }
    else {
        return (x + (y * wstate->sq_count_x));
    }
}

/* we calculate from the bottom left to top right neighbours */
int next_neighbour_index(struct square *s,
		struct window_state *wstate,
		double size,
		int k){
    switch (k) {
        case 0: return get_valid_index_if_inside(wstate, s->index_x - 1, s->index_y - 1);
        case 1: return get_valid_index_if_inside(wstate, s->index_x    , s->index_y - 1);
        case 2: return get_valid_index_if_inside(wstate, s->index_x + 1, s->index_y - 1);

        case 3: return get_valid_index_if_inside(wstate, s->index_x - 1, s->index_y    );
        case 4: return -2; // this is you
        case 5: return get_valid_index_if_inside(wstate, s->index_x + 1, s->index_y    );

        case 6: return get_valid_index_if_inside(wstate, s->index_x - 1, s->index_y  + 1);
        case 7: return get_valid_index_if_inside(wstate, s->index_x    ,  s->index_y  + 1);
        case 8: return get_valid_index_if_inside(wstate, s->index_x + 1, s->index_y  + 1);
    }
}
#endif

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
	double xxx = ((x_end - x_start) * (y_end - y_start) * 256);
	rAvg/=xxx;
	gAvg/=xxx;
	bAvg/=xxx;
	set_color(&gwin->cstate[sq_number], rAvg, gAvg, bAvg);
}

static void show_neighbours(int index, int *neighbours){
	printf(" SQ %d has ", index);
	for(int i= 0 ; i < 9; i++){
		printf(" %d ", neighbours[i]);
	}
	printf("\n");
	fflush(NULL);
}

void calculate_next_generation_wikialgo(struct global_win1 *gwin, int index)
{
    int neighbours[9], total_alive = 0, total_dead = 0;
    struct square *s = &gwin->squares[index];
    struct gol_state *gstate = gwin->gstate;
    for(int i= 0 ; i < 9; i++){
        neighbours[i] = next_neighbour_index(s,
        		&gwin->wstate,
        		gwin->wstate.size,
        		i);
    }
    //show_neighbours(index, neighbours);
    for(int i= 0 ; i < 9; i++){
    	assert(neighbours[i] != index);
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

void assign_gol_colors(struct gol_state *gstate, struct color_state *cstate, int items){
	/* now we need to draw - assign colors first */
	for(int i = 0 ;i < items; i++){
		if(gstate[i].isAlive)
			set_white(&cstate[i]);
		else
			set_black(&cstate[i]);
	}
}

/* it is the responsibility of the caller to set the right window */
void run_scan_for_wikialgo(struct global_win1 *gwin){
    /* clean next generation */
	int items = gwin->wstate.sq_total;
	for(int i = 0 ;i < items; i++){
		gwin->gstate[i].nx_isAlive = 0;
	}
	/* calculate the next generation */
	for(int i = 0 ; i < items; i++){
		calculate_next_generation_wikialgo(gwin, i);
	}
	/* now we swap */
	for(int i =0; i < items; i++) {
		gwin->gstate[i].isAlive = gwin->gstate[i].nx_isAlive;
	}
	// we assign color here because timer need to be independent.
	// timer calls this and display() displays the RGB values
	assign_gol_colors(gwin->gstate,
	    		gwin->cstate,
	    		gwin->wstate.sq_total);
}

void calculate_next_generation_wikialgo_xbox(struct global_win1 *gwin, int index)
{
    int neighbours[9], total_alive = 0, total_dead = 0;
    struct square *s = &gwin->squares[index];
    struct gol_state *gstate = gwin->gstate;
    /* we calculate these averages anyways */
    double alive_avg_r=0.0, alive_avg_g=0.0, alive_avg_b=0.0;
    for(int i= 0 ; i < 9; i++){
        neighbours[i] = next_neighbour_index(s,
        		&gwin->wstate,
        		gwin->wstate.size,
        		i);
    }
    //show_neighbours(index, neighbours);
    for(int i= 0 ; i < 9; i++){
    	assert(neighbours[i] != index);
        /* for all the valid entries */
        if(neighbours[i] >= 0){
            if(gstate[neighbours[i]].isAlive){
            	/* if alive then we add them up for average */
            	alive_avg_r+=gwin->cstate[neighbours[i]].r;
            	alive_avg_g+=gwin->cstate[neighbours[i]].g;
            	alive_avg_b+=gwin->cstate[neighbours[i]].b;
                total_alive++;
            } else {
                total_dead++;
            }
        }
    }
    printf(" alive: %d dead %d ", total_alive, total_dead);
    alive_avg_r/=total_alive;
    alive_avg_g/=total_alive;
    alive_avg_b/=total_alive;

    /* for this square the next state is */
    if(gstate[index].isAlive){
        /* if this was alive */
        if(total_alive < 2){
        	gstate[index].nx_isAlive = 0; // dead, underpopulation
        } else if (total_alive >= 2 && total_alive <= 3){
        	/* here we use the average for the next generation */
        	gwin->cstate[index].nx_r = alive_avg_r;
        	gwin->cstate[index].nx_g = alive_avg_g;
        	gwin->cstate[index].nx_b = alive_avg_b;
        	gstate[index].nx_isAlive = 1; // ok
        	printf(" marking %d alive \n", index);
        } else if (total_alive > 3){
        	gstate[index].nx_isAlive = 0; // ok, over-population
        } else {
            //printf(" ### we stay alive? total_alive %d total_dead %d \n", total_alive, total_dead);
        }
    } else {
        /* we were dead */
        if(total_alive == 3){
        	/* here we use the average for the next generation */
        	gwin->cstate[index].nx_r = alive_avg_r;
        	gwin->cstate[index].nx_g = alive_avg_g;
        	gwin->cstate[index].nx_b = alive_avg_b;
        	gstate[index].nx_isAlive = 1; // re-surrected
        	printf(" marking %d alive \n", index);
        } else {
        }
    }
}

void run_scan_for_wikialgo_xbox(struct global_win1 *gwin){
    /* clean next generation state and color */
	int items = gwin->wstate.sq_total;
	for(int i = 0 ;i < items; i++){
		gwin->gstate[i].nx_isAlive = 0;
		gwin->cstate[i].nx_r = gwin->cstate[i].nx_g =  gwin->cstate[i].nx_b = 0.0;
	}

	/* calculate the next generation state and color */
	for(int i = 0 ; i < items; i++){
		calculate_next_generation_wikialgo_xbox(gwin, i);
	}

	/* now we swap the color and GOL states */
	for(int i =0; i < items; i++) {
		gwin->gstate[i].isAlive = gwin->gstate[i].nx_isAlive;
		if(gwin->gstate[i].isAlive){
			/* only if you are alive */
			gwin->cstate[i].r = gwin->cstate[i].nx_r;
			gwin->cstate[i].g = gwin->cstate[i].nx_g;
			gwin->cstate[i].b = gwin->cstate[i].nx_b;
		}
	}
}

static void _run_scan_for_xboxdata(struct global_win1 *gwin, uint8_t *data){
	// make sure no access to the GOL state
	int items = gwin->wstate.sq_total;
	/* calculate the next generation */
	for(int i = 0 ; i < items; i++){
		calculate_avg_color_from_xboxdata(gwin, i, data);
	}
}

void run_scan_for_xboxdata_depth(struct global_win1 *gwin){
	_run_scan_for_xboxdata(gwin, depth_front);
	//_run_scan_for_xboxdata(gwin, rgb_front);
}

void run_scan_for_xboxdata_natural(struct global_win1 *gwin){
	_run_scan_for_xboxdata(gwin, rgb_front);
}

static double value_map_size(char *seq, long gen_number){
#if 0
	if(!strcmp(seq, "RGB")){
		return 0.01; // reg gained mostly from gree - close
	} else if(!strcmp(seq, "RBG")){
		return 0.03; // reg gained mostly from Blue - close
	} else if(!strcmp(seq, "GRB")){
		return 0.05; // green gained mostly from R - away
	} else if(!strcmp(seq, "GBR")){
		return 0.04; // green gained mostly from blue -- close
	} else if(!strcmp(seq, "BRG")){
		return 0.07; // blue gained mostly from red -- away
	} else if(!strcmp(seq, "BGR")){
		return 0.1; // blue gained mostly from green -- away
	} else {
		return 0.1;
	}
#endif
	int x = gen_number % 5;
	if(x == 0)
		return 0.01;
	else if (x == 1)
    	return 0.03;
	else if (x == 2)
	    return 0.05;
	else if (x == 3)
		 return 0.07;
	else
		 return 0.1;
}

static int value_map_timeout(char *seq){
	if(!strcmp(seq, "RGB")){
		return 10;
	} else if(!strcmp(seq, "RBG")){
		return 50;
	} else if(!strcmp(seq, "GRB")){
		return 100;
	} else if(!strcmp(seq, "GBR")){
		return 250;
	} else if(!strcmp(seq, "BRG")){
		return 500;
	} else if(!strcmp(seq, "BGR")){
		return 1000;
	} else {
		return 1000;
	}
}

static void calculate_order_and_sequence(struct zoom_state *zstate){
	zstate->diff_r = zstate->r - zstate->last_r;
	zstate->diff_g = zstate->g - zstate->last_g;
	zstate->diff_b = zstate->b - zstate->last_b;
	int sum = zstate->diff_r + zstate->diff_g + zstate->diff_b;
	printf(" last (%d, %d, %d) now (%d, %d, %d) || GAINS (%d, %d, %d) || sum %d \n",
				zstate->last_r,
				zstate->last_g,
				zstate->last_b,
				zstate->r,
				zstate->g,
				zstate->b,
				zstate->diff_r,
				zstate->diff_g,
				zstate->diff_b,
				sum);
	if(zstate->diff_r >= zstate->diff_g){
		if( zstate->diff_r >= zstate->diff_b){
			// R is the max
			if( zstate->diff_g >= zstate->diff_b)
				zstate->seq = "RGB";
			else
				zstate->seq = "RBG";
		} else {
			// here we know that B was bigger than R and R was bigger than G
			zstate->seq  = "BRG";
		}
	} else {
		// we know that G > R
		if( zstate->diff_r >= zstate->diff_b){
			// R > B
			zstate->seq = "GRB";
		} else {
			// B > R and G > R
			if( zstate->diff_g >= zstate->diff_b){
				zstate->seq = "GBR";
			} else {
				zstate->seq = "BGR";
			}
		}
	}
}

static void calculate_rgb_deltas(struct global_win1 *gwin){
	// since the zoom factor of gwin is variable we need to scan the raw
	// data to figure out how many pixels have changed their color on the
	// spectrum. Also this ensures that the diff gains are on a zero-sum
	// scale
	int now_r = 0, now_g = 0, now_b = 0;
	uint8_t r, g, b;
	for(int i = 0; i < 480; i++){
		for(int j = 0; j < 640; j++){
			//Flat[x + WIDTH * (y + DEPTH * z)] = Original[x, y, z]
			// Original[HEIGHT, WIDTH, DEPTH] then
			//         [480, 640, 3]
			int index = ((i*3) * 640 + (j*3));
			r=rgb_front[index];
			g=rgb_front[index + 1];
			b=rgb_front[index + 2];
			if(r > g){
				if( r > b)
					now_r++;
				else
					now_b++;
			} else {
				if(g > b)
					now_g++;
				else
					now_b++;
			}
		}
	}
	// now we know how many new pixels are there
	gwin->zstate.last_r = gwin->zstate.r;
	gwin->zstate.last_g = gwin->zstate.g;
	gwin->zstate.last_b = gwin->zstate.b;

	gwin->zstate.r = now_r;
	gwin->zstate.g = now_g;
	gwin->zstate.b = now_b;
	calculate_order_and_sequence(&gwin->zstate);
}

double calculate_zoom_size(struct global_win1 *gwin, long gen_number){
	calculate_rgb_deltas(gwin);
	return value_map_size(gwin->zstate.seq, gen_number);
}

int calculate_timeout_wait(struct global_win1 *gwin){
	calculate_rgb_deltas(gwin);
	return value_map_timeout(gwin->zstate.seq);
}

#define MAX_BITS 4
#define MAX_COLORS (1U << MAX_BITS)
#define MAX_GAP 8

int matches(uint8_t value, uint8_t to){
	uint8_t vx = (value + 1) % MAX_COLORS; // let it over flow
	return abs(vx - to) <= MAX_GAP;

}
int calculate_cyclic_number(double _r, double _g, double _b, int mode){
	int r = _r * 255;
	int g = _g * 255;
	int b = _b * 255;
	int color;
	int r_bits = MAX_BITS/3;
	int g_bits = MAX_BITS/3;
	int b_bits = MAX_BITS - (r_bits + g_bits);
	int r_mask = 1 << r_bits;
	int g_mask = 1 << g_bits;
	int b_mask = 1 << b_bits;

	color = ((r % r_mask) << (g_bits + b_bits)) + ((g % g_mask) << b_bits) + (b % b_mask);
#if 0
	if(mode == 1){
		color = (r * 7 / 255) << 5 + (g * 7 / 255) << 2 + (b * 3 / 255);
	} else if (mode == 2) {
		color = (r*6/256)*36 + (g*6/256)*6 + (b*6/256);
	} else {
		uint8_t color = ((r%8) << 5) + ((g%8) << 2) + (b%4);
	}
#endif
	//printf(" mapping %f %f %f | %d %d %d -> to %d || r_bits %d g_bits %d b_bits %d \n",
	//		_r, _g, _b, r, g, b, color, r_bits, g_bits, b_bits);
	assert(color <= MAX_COLORS);
	return color;
}


void calculate_next_generation_cyclic_colors(struct global_win1 *gwin, int index)
{
    int neighbours[9];
    int found = 0;
    struct square *s = &gwin->squares[index];
    struct color_state *c = gwin->cstate;
    for(int i= 0 ; i < 9; i++){
        neighbours[i] = next_neighbour_index(s,
        		&gwin->wstate,
        		gwin->wstate.size,
        		i);
    }
    //show_neighbours(index, neighbours);
    // for all the neighbours we scan
    for(int i= 0 ; i < 9; i++){
    	assert(neighbours[i] != index);
        /* for all the valid entries */
        if(neighbours[i] >= 0){
            if(matches(c[index].colx, c[neighbours[i]].colx)){
            	// we match and hence we inherit */
            	c[index].nx_colx = c[neighbours[i]].colx;
            	c[index].nx_r = c[neighbours[i]].r;
            	c[index].nx_g = c[neighbours[i]].g;
            	c[index].nx_b = c[neighbours[i]].b;
            	c[index].changed = 1;
            	//printf(" copying colors from %d to %d \n", neighbours[i], index);
            	break;
            }
        }
    }
}

void run_scan_for_cyclic_automaton(struct global_win1 *gwin){
    /* clean next generation state and color */
	int items = gwin->wstate.sq_total;
	for(int i = 0 ;i < items; i++){
		gwin->cstate[i].nx_colx = 0;
		gwin->cstate[i].changed = 0;
	}

	/* calculate the next generation state and color */
	for(int i = 0 ; i < items; i++){
		calculate_next_generation_cyclic_colors(gwin, i);
	}

	/* now we swap the cyclic colors */
	for(int i =0; i < items; i++) {
		if(gwin->cstate[i].changed){
			gwin->cstate[i].r = gwin->cstate[i].nx_r;
			gwin->cstate[i].g = gwin->cstate[i].nx_g;
			gwin->cstate[i].b = gwin->cstate[i].nx_b;
		}
	}
}

double get_next_double(){
	double range = (1.0 - 0);
	double div = RAND_MAX / range;
	return 0.0 + (rand() / div);
}

int get_next_int(int min, int max){
	return rand() % (max + 1 - min) + min;
}
