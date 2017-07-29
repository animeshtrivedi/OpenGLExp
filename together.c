//
// Created by atr on 06.07.17.
//

//
// Created by atr on 06.07.17.
//

/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libfreenect.h"

#include <pthread.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

int window_xbox, window_gol1;

// back: owned by libfreenect (implicit for depth)
// mid: owned by callbacks, "latest frame ready"
// front: owned by GL, "currently being drawn"
uint8_t *depth_mid, *depth_front;
uint8_t *rgb_back, *rgb_mid, *rgb_front;

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

// works first example!
//int init_state[] = {7, 12, 17};
//#define SIZEX 0.4

// works! -- second example
//int init_state[] = {13,14,15,20,21,22};
//#define SIZEX 0.3

// example 3 - works
int init_state[] = {9,10,15,16,19,20,25,26};
#define SIZEX 0.3

/* current height and width of the window : initial 1024 x 768 */
//int curr_height = 1024;
//int curr_width = 768;

static double bottom_left_x = -1.0;
static double bottom_left_y = -1.0;

static double top_right_x = 1.0;
static double top_right_y = 1.0;

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
int init_calculate_rectangles_num(){

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
    /* we start from bottom left side */
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


/* set colors on squares */
void set_black(struct square *s){
    s->r = s->g = s->b= 0.0;
}

void set_white(struct square *s){
    s->r = s->g = s->b= 0.8;
}

void set_color(struct square *s, double r, double g, double b){
    s->r = r;
    s->g = g;
    s->b = b;
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
#if 1
        if(s->isAlive){
            glColor3f(0.5f, 0.0f, 0.0f);
        } else {
            glColor3f(0.9f,0.9f,0.9f);
        }
#else
        glColor3f(s->r, s->g, s->b);
#endif

        glRectf(x1, y1, x2, y2);
#endif
        /* move to the next square */
        s++;
    }
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

int calculate_avg_from_xboxdata(struct square *s, int index, uint8_t *data) {
	/* s is the entry in the global array */
	/* we need to find x and y ranges in 640 x 480 */

	double maxXRange = top_right_x - bottom_left_x;
	double maxYRange = top_right_y - bottom_left_y;

	double xMultiplier = 640 / maxXRange;
	double yMultiplier = 480 / maxYRange;

	int x_start = (s->x - bottom_left_x) * xMultiplier;
	int x_end = (s->x + s->s - bottom_left_x) * xMultiplier;

	int y_start = (s->y - bottom_left_y) * yMultiplier;
	int y_end = (s->y + s->s - bottom_left_y) * yMultiplier;

	assert(x_start >= 0 && x_start <= 640);
	assert(x_end >= 0 && x_end <= 640);

	assert(y_start >= 0 && y_start <= 480);
	assert(y_end >= 0 && y_end <= 480);

	double rAvg = 0 , gAvg = 0 , bAvg = 0;

	int ylimit = y_end - y_start;
	int xlimit = x_end - y_end;
	int items = 640 * 480;
	for(int i = x_start; i <= x_end; i++){
		for(int j = y_start; j <= y_end; j++){
			// Flat[x + WIDTH * (y + DEPTH * z)] = Original[x, y, z]
			// https://stackoverflow.com/questions/7367770/how-to-flatten-or-index-3d-array-in-1d-array
#if 1
			/* most likely - it is this encoding RGB,RGB,RGB,RGB...
			 * somehow this is flipped - I,i) indexes are accessed like this.
			 * This 480 -j come from the fact that OpenGL rectangle
			 * calculations starts at the bottom left, whereas RGB color data
			 * starts at top left. Hence (480 -j)
			 */
	        int index = ((480 - j) * 640 * 3) + (i * 3);
	        rAvg+=data[index + 0];
	        gAvg+=data[index + 1];
	        bAvg+=data[index + 2];
#else
	          /* or alternatively : RRRR...GGG...BBB*/
	          rAvg+=data[i * 640 + j];
	          gAvg+=data[(items) + i * 640 + j];
	          bAvg+=data[(items * 2) + i * 640 + j];
#endif
		}
	}
	// if the value is byte then to normalize it we must divide by 255
	double xxx = ((x_end - x_start) * (y_end - y_start) * 255);
	rAvg/=xxx;
	gAvg/=xxx;
	bAvg/=xxx;
	printf(" Setting average color to (index: %d) : %f %f %f \n", index, rAvg, gAvg, bAvg);
	set_color(s, rAvg, gAvg, bAvg);
}

int calculate_next_generation(struct square *s, int index)
{
    int neix[9], total_alive = 0, total_dead = 0, stx;
    for(int i= 0 ; i < 9; i++){
        neix[i] = next_neighbour_index(s, i);
    }
    //printf(" Index %d has neighbours as : ", s->index);
    for(int i= 0 ; i < 9; i++){
        //printf(" : %d ", neix[i]);
        /* for all the valid entries */
        if(neix[i] >= 0){
            if(SX[neix[i]].isAlive){
                total_alive++;
            } else {
                total_dead++;
            }
        }
    }
    //printf(" || total_alive %d , total_dead: %d alive? %d ", total_alive, total_dead, s->isAlive);
    /* for this square the next state is */
    if(s->isAlive){
        /* if this was alive */
        if(total_alive < 2){
            next_generation_status[index] = 0; // dead, underpopulation
            //printf(" result: dead (underpopulation) \n");
        } else if (total_alive >= 2 && total_alive <= 3){
            next_generation_status[index] = 1; // ok
            //printf(" result: survive ! (OK) \n");
        } else if (total_alive > 3){
            next_generation_status[index] = 0; // ok, over-population
            //printf(" result: dead (over population) \n");
        } else {
            //printf(" ### we stay alive? total_alive %d total_dead %d \n", total_alive, total_dead);
        }
    } else {
        /* we were dead */
        if(total_alive == 3){
            next_generation_status[index] = 1; // re-surrected
            //printf(" result: alive (resurrected) \n");
        } else {
            //printf(" ### we stay dead? total_alive %d total_dead %d \n", total_alive, total_dead);
        }
    }
    return 0;
}

int run_scan(){
    /* clean next generation */
    bzero(next_generation_status, sizeof(int) * sq_total);

    /* calculate next generation status */
    for(int i =0; i < sq_total; i++){
#if 1
        calculate_next_generation(SX + i, i);
#else
    	calculate_avg_from_xboxdata(SX + i, i, depth_front);
    	//calculate_avg_from_xboxdata(SX + i, i, rgb_front);
#endif
    }
    /* apply next generation */
    for(int i =0; i < sq_total; i++){
        SX[i].isAlive = next_generation_status[i];
    }
}


static void init (void)
{
    //glutSetWindow(window_gol);
    init_calculate_rectangles_num();
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void display()
{
    glutSetWindow(window_gol1);
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    walk_and_draw_rectangles();
    glutSwapBuffers();
}

static void timer_redraw_1(int value){
    printf(" Execution for generation %ld \n", global_time);
    glutSetWindow(window_gol1);
    run_scan();
    glutPostRedisplay();
    // 1000 milliseconds
    global_time++;
    glutTimerFunc(1000, timer_redraw_1, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
pthread_t freenect_thread;
volatile int die = 0;

int g_argc;
char **g_argv;

int window;

pthread_mutex_t gl_backbuf_mutex = PTHREAD_MUTEX_INITIALIZER;


GLuint gl_depth_tex;
GLuint gl_rgb_tex;
GLfloat camera_angle = 0.0;
int camera_rotate = 0;
int tilt_changed = 0;

freenect_context *f_ctx;
freenect_device *f_dev;
int freenect_angle = 0;
int freenect_led;

freenect_video_format requested_format = FREENECT_VIDEO_RGB;
freenect_video_format current_format = FREENECT_VIDEO_RGB;

pthread_cond_t gl_frame_cond = PTHREAD_COND_INITIALIZER;
int got_rgb = 0;
int got_depth = 0;

void DrawGLScene()
{
    pthread_mutex_lock(&gl_backbuf_mutex);
    glutSetWindow(window_xbox);

    // When using YUV_RGB mode, RGB frames only arrive at 15Hz, so we shouldn't force them to draw in lock-step.
    // However, this is CPU/GPU intensive when we are receiving frames in lockstep.
    if (current_format == FREENECT_VIDEO_YUV_RGB) {
        while (!got_depth && !got_rgb) {
            pthread_cond_wait(&gl_frame_cond, &gl_backbuf_mutex);
        }
    } else {
        while ((!got_depth || !got_rgb) && requested_format != current_format) {
            pthread_cond_wait(&gl_frame_cond, &gl_backbuf_mutex);
        }
    }

    if (requested_format != current_format) {
        pthread_mutex_unlock(&gl_backbuf_mutex);
        return;
    }

    uint8_t *tmp;

    if (got_depth) {
        tmp = depth_front;
        depth_front = depth_mid;
        depth_mid = tmp;
        got_depth = 0;
    }
    if (got_rgb) {
        tmp = rgb_front;
        rgb_front = rgb_mid;
        rgb_mid = tmp;
        got_rgb = 0;
    }

    pthread_mutex_unlock(&gl_backbuf_mutex);
    glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, depth_front);

    if (camera_rotate) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        freenect_raw_tilt_state* state;
        freenect_update_tilt_state(f_dev);
        state = freenect_get_tilt_state(f_dev);
        GLfloat x_accel_raw, x_accel,y_accel_raw,y_accel;
        x_accel_raw = (GLfloat)state->accelerometer_x/819.0;
        y_accel_raw = (GLfloat)state->accelerometer_y/819.0;

        // sloppy acceleration vector cleanup
        GLfloat accel_length = sqrt(x_accel_raw * x_accel_raw + y_accel_raw * y_accel_raw);
        x_accel = x_accel_raw/accel_length;
        y_accel = y_accel_raw/accel_length;
        camera_angle = atan2(y_accel,x_accel)*180/M_PI -90.0;
    }
    else {
        camera_angle = 0;
    }

    glLoadIdentity();
    glPushMatrix();
    glTranslatef((640.0/2.0),(480.0/2.0) ,0.0);
    glRotatef(camera_angle, 0.0, 0.0, 1.0);
    glTranslatef(-(640.0/2.0),-(480.0/2.0) ,0.0);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glTexCoord2f(0, 1); glVertex3f(0,0,1.0);
    glTexCoord2f(1, 1); glVertex3f(640,0,1.0);
    glTexCoord2f(1, 0); glVertex3f(640,480,1.0);
    glTexCoord2f(0, 0); glVertex3f(0,480,1.0);
    glEnd();
    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
    if (current_format == FREENECT_VIDEO_RGB || current_format == FREENECT_VIDEO_YUV_RGB) {
        glTexImage2D(GL_TEXTURE_2D, 0, 3, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_front);
    } else {
        // atr: this is not used
        glTexImage2D(GL_TEXTURE_2D, 0, 1, 640, 480, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, rgb_front + 640 * 4);
    }

    glPushMatrix();
    glTranslatef(640+(640.0/2.0),(480.0/2.0) ,0.0);
    glRotatef(camera_angle, 0.0, 0.0, 1.0);
    glTranslatef(-(640+(640.0/2.0)),-(480.0/2.0) ,0.0);

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glTexCoord2f(0, 1); glVertex3f(640,0,0);
    glTexCoord2f(1, 1); glVertex3f(1280,0,0);
    glTexCoord2f(1, 0); glVertex3f(1280,480,0);
    glTexCoord2f(0, 0); glVertex3f(640,480,0);
    glEnd();
    glPopMatrix();
    glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y)
{
    glutSetWindow(window_xbox);
    if (key == 27) {
        die = 1;
        pthread_join(freenect_thread, NULL);
        glutDestroyWindow(window);
        free(depth_mid);
        free(depth_front);
        free(rgb_back);
        free(rgb_mid);
        free(rgb_front);
        // Not pthread_exit because OSX leaves a thread lying around and doesn't exit
        exit(0);
    }
    if (key == 'w') {
        freenect_angle++;
        if (freenect_angle > 30) {
            freenect_angle = 30;
        }
        tilt_changed++;
    }
    if (key == 's') {
        freenect_angle = 0;
        tilt_changed++;
    }
    if (key == 'f') {
        if (requested_format == FREENECT_VIDEO_IR_8BIT)
            requested_format = FREENECT_VIDEO_RGB;
        else if (requested_format == FREENECT_VIDEO_RGB)
            requested_format = FREENECT_VIDEO_YUV_RGB;
        else
            requested_format = FREENECT_VIDEO_IR_8BIT;
    }
    if (key == 'x') {
        freenect_angle--;
        if (freenect_angle < -30) {
            freenect_angle = -30;
        }
        tilt_changed++;
    }
    if (key == 'e') {
        static freenect_flag_value auto_exposure = FREENECT_ON;
        freenect_set_flag(f_dev, FREENECT_AUTO_EXPOSURE, auto_exposure);
        auto_exposure = auto_exposure ? FREENECT_OFF : FREENECT_ON;
    }
    if (key == 'b') {
        static freenect_flag_value white_balance = FREENECT_ON;
        freenect_set_flag(f_dev, FREENECT_AUTO_WHITE_BALANCE, white_balance);
        white_balance = white_balance ? FREENECT_OFF : FREENECT_ON;
    }
    if (key == 'r') {
        static freenect_flag_value raw_color = FREENECT_ON;
        freenect_set_flag(f_dev, FREENECT_RAW_COLOR, raw_color);
        raw_color = raw_color ? FREENECT_OFF : FREENECT_ON;
    }
    if (key == 'm') {
        static freenect_flag_value mirror = FREENECT_ON;
        freenect_set_flag(f_dev, FREENECT_MIRROR_DEPTH, mirror);
        freenect_set_flag(f_dev, FREENECT_MIRROR_VIDEO, mirror);
        mirror = mirror ? FREENECT_OFF : FREENECT_ON;
    }
    if (key == 'n') {
        static freenect_flag_value near_mode = FREENECT_ON;
        freenect_set_flag(f_dev, FREENECT_NEAR_MODE, near_mode);
        near_mode = near_mode ? FREENECT_OFF : FREENECT_ON;
    }

    if (key == '+') {
        uint16_t brightness = freenect_get_ir_brightness(f_dev) + 2;
        freenect_set_ir_brightness(f_dev, brightness);
    }
    if (key == '-') {
        uint16_t brightness = freenect_get_ir_brightness(f_dev) - 2;
        freenect_set_ir_brightness(f_dev, brightness);
    }

    if (key == '1') {
        freenect_set_led(f_dev,LED_GREEN);
    }
    if (key == '2') {
        freenect_set_led(f_dev,LED_RED);
    }
    if (key == '3') {
        freenect_set_led(f_dev,LED_YELLOW);
    }
    if (key == '4') {
        freenect_set_led(f_dev,LED_BLINK_GREEN);
    }
    if (key == '5') {
        // 5 is the same as 4
        freenect_set_led(f_dev,LED_BLINK_GREEN);
    }
    if (key == '6') {
        freenect_set_led(f_dev,LED_BLINK_RED_YELLOW);
    }
    if (key == '0') {
        freenect_set_led(f_dev,LED_OFF);
    }

    if (key == 'o') {
        if (camera_rotate) {
            camera_rotate = 0;
            glDisable(GL_DEPTH_TEST);
        }
        else {
            camera_rotate = 1;
            glEnable(GL_DEPTH_TEST);
        }
    }
    if (tilt_changed) {
        freenect_set_tilt_degs(f_dev, freenect_angle);
        tilt_changed = 0;
    }
}

void ReSizeGLScene(int Width, int Height)
{
    glutSetWindow(window_xbox);
    glViewport(0,0,Width,Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (0, 1280, 0, 480, -5.0f, 5.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void InitGL(int Width, int Height)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glClearDepth(0.0);
    //glDepthFunc(GL_LESS);
    //glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_FLAT);

    glGenTextures(1, &gl_depth_tex);
    glBindTexture(GL_TEXTURE_2D, gl_depth_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &gl_rgb_tex);
    glBindTexture(GL_TEXTURE_2D, gl_rgb_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ReSizeGLScene(Width, Height);
}

void *gl_threadfunc(void *arg)
{
    printf("GL thread\n");

    glutInit(&g_argc, g_argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
    glutInitWindowSize(1280, 480);
    glutInitWindowPosition(0, 0);

    window = window_xbox = glutCreateWindow("LibFreenect");

    glutDisplayFunc(&DrawGLScene);
    glutIdleFunc(&DrawGLScene);
    glutReshapeFunc(&ReSizeGLScene);
    glutKeyboardFunc(&keyPressed);

    InitGL(1280, 480);

    /* we can put next screen here */
    //glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0, 550);
    window_gol1 = glutCreateWindow ("Welcome to the Life !");
    glutKeyboardFunc(&keyPressed);
    init();
    glutDisplayFunc(&display);
    glutTimerFunc(1000, timer_redraw_1, 0);

    glutMainLoop();

    return NULL;
}

uint16_t t_gamma[2048];

void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
    int i;
    uint16_t *depth = (uint16_t*)v_depth;

    pthread_mutex_lock(&gl_backbuf_mutex);
    for (i=0; i<640*480; i++) {
        int pval = t_gamma[depth[i]];
        int lb = pval & 0xff;
        switch (pval>>8) {
            case 0:
                depth_mid[3*i+0] = 255;
                depth_mid[3*i+1] = 255-lb;
                depth_mid[3*i+2] = 255-lb;
                break;
            case 1:
                depth_mid[3*i+0] = 255;
                depth_mid[3*i+1] = lb;
                depth_mid[3*i+2] = 0;
                break;
            case 2:
                depth_mid[3*i+0] = 255-lb;
                depth_mid[3*i+1] = 255;
                depth_mid[3*i+2] = 0;
                break;
            case 3:
                depth_mid[3*i+0] = 0;
                depth_mid[3*i+1] = 255;
                depth_mid[3*i+2] = lb;
                break;
            case 4:
                depth_mid[3*i+0] = 0;
                depth_mid[3*i+1] = 255-lb;
                depth_mid[3*i+2] = 255;
                break;
            case 5:
                depth_mid[3*i+0] = 0;
                depth_mid[3*i+1] = 0;
                depth_mid[3*i+2] = 255-lb;
                break;
            default:
                depth_mid[3*i+0] = 0;
                depth_mid[3*i+1] = 0;
                depth_mid[3*i+2] = 0;
                break;
        }
    }
    got_depth++;
    pthread_cond_signal(&gl_frame_cond);
    pthread_mutex_unlock(&gl_backbuf_mutex);
}

void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
    pthread_mutex_lock(&gl_backbuf_mutex);

    // swap buffers
    assert (rgb_back == rgb);
    rgb_back = rgb_mid;
    freenect_set_video_buffer(dev, rgb_back);
    rgb_mid = (uint8_t*)rgb;

    got_rgb++;
    pthread_cond_signal(&gl_frame_cond);
    pthread_mutex_unlock(&gl_backbuf_mutex);
}

void *freenect_threadfunc(void *arg)
{
    int accelCount = 0;

    freenect_set_tilt_degs(f_dev,freenect_angle);
    freenect_set_led(f_dev,LED_RED);
    freenect_set_depth_callback(f_dev, depth_cb);
    freenect_set_video_callback(f_dev, rgb_cb);
    freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, current_format));
    freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));
    freenect_set_video_buffer(f_dev, rgb_back);

    freenect_start_depth(f_dev);
    freenect_start_video(f_dev);

    printf("'w' - tilt up, 's' - level, 'x' - tilt down, '0'-'6' - select LED mode, '+' & '-' - change IR intensity \n");
    printf("'f' - change video format, 'm' - mirror video, 'o' - rotate video with accelerometer \n");
    printf("'e' - auto exposure, 'b' - white balance, 'r' - raw color, 'n' - near mode (K4W only) \n");

    while (!die && freenect_process_events(f_ctx) >= 0) {
        //Throttle the text output
        if (accelCount++ >= 2000)
        {
            accelCount = 0;
            freenect_raw_tilt_state* state;
            freenect_update_tilt_state(f_dev);
            state = freenect_get_tilt_state(f_dev);
            double dx,dy,dz;
            freenect_get_mks_accel(state, &dx, &dy, &dz);
            printf("\r raw acceleration: %4d %4d %4d  mks acceleration: %4f %4f %4f", state->accelerometer_x, state->accelerometer_y, state->accelerometer_z, dx, dy, dz);
            fflush(stdout);
        }

        if (requested_format != current_format) {
            freenect_stop_video(f_dev);
            freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, requested_format));
            freenect_start_video(f_dev);
            current_format = requested_format;
        }
    }

    printf("\nshutting down streams...\n");

    freenect_stop_depth(f_dev);
    freenect_stop_video(f_dev);

    freenect_close_device(f_dev);
    freenect_shutdown(f_ctx);

    printf("-- done!\n");
    return NULL;
}

int together_main(int argc, char **argv)
{
    int res;

    depth_mid = (uint8_t*)malloc(640*480*3);
    depth_front = (uint8_t*)malloc(640*480*3);
    rgb_back = (uint8_t*)malloc(640*480*3);
    rgb_mid = (uint8_t*)malloc(640*480*3);
    rgb_front = (uint8_t*)malloc(640*480*3);

    printf("Kinect camera test\n");

    int i;
    for (i=0; i<2048; i++) {
        float v = i/2048.0;
        v = powf(v, 3)* 6;
        t_gamma[i] = v*6*256;
    }

    g_argc = argc;
    g_argv = argv;

    if (freenect_init(&f_ctx, NULL) < 0) {
        printf("freenect_init() failed\n");
        return 1;
    }

    freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
    freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

    int nr_devices = freenect_num_devices (f_ctx);
    printf ("Number of devices found: %d\n", nr_devices);

    int user_device_number = 0;
    if (argc > 1)
        user_device_number = atoi(argv[1]);

    if (nr_devices < 1) {
        freenect_shutdown(f_ctx);
        return 1;
    }

    if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
        printf("Could not open device\n");
        freenect_shutdown(f_ctx);
        return 1;
    }

    res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
    if (res) {
        printf("pthread_create failed\n");
        freenect_shutdown(f_ctx);
        return 1;
    }

    // OS X requires GLUT to run on the main thread
    gl_threadfunc(NULL);

    return 0;
}
