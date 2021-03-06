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


static pthread_mutex_t gl_backbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

uint8_t *depth_mid, *depth_front;
uint8_t *rgb_back, *rgb_mid, *rgb_front;
static uint16_t t_gamma[2048];

static pthread_t freenect_thread;
static volatile int die;
static int window;
static GLuint gl_depth_tex;
static GLuint gl_rgb_tex;
static GLfloat camera_angle = 0.0;
static int camera_rotate = 0;
static int tilt_changed = 0;

static freenect_context *f_ctx;
static freenect_device *f_dev;
static int freenect_angle = 0;
static int freenect_led;

static freenect_video_format requested_format = FREENECT_VIDEO_RGB;
static freenect_video_format current_format = FREENECT_VIDEO_RGB;

static pthread_cond_t gl_frame_cond = PTHREAD_COND_INITIALIZER;
static int got_rgb = 0;
static int got_depth = 0;

static void DrawGLScene()
{
	glutSetWindow(window);
    pthread_mutex_lock(&gl_backbuf_mutex);

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

static void keyPressed(unsigned char key, int x, int y)
{
	glutSetWindow(window);
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

static void ReSizeGLScene(int Width, int Height)
{
	glutSetWindow(window);
    glViewport(0,0,Width,Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (0, 1280, 0, 480, -5.0f, 5.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void InitGL(int Width, int Height)
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

static void *gl_threadfunc(int argc, char **argv)
{
	// this will become the main function that sets things up
    printf("GL thread\n");

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
    glutInitWindowSize(1280, 480);
    glutInitWindowPosition(0, 0);

    window = glutCreateWindow("LibFreenect");

    glutDisplayFunc(&DrawGLScene);
    glutIdleFunc(&DrawGLScene);
    glutReshapeFunc(&ReSizeGLScene);
    glutKeyboardFunc(&keyPressed);
    InitGL(1280, 480);
    return NULL;
}

static void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
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

static void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
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

static void *freenect_threadfunc(void *arg)
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

int init_xbox(int argc, char **argv) {
    int res;

    depth_mid = (uint8_t*)malloc(640*480*3);
    depth_front = (uint8_t*)malloc(640*480*3);
    rgb_back = (uint8_t*)malloc(640*480*3);
    rgb_mid = (uint8_t*)malloc(640*480*3);
    rgb_front = (uint8_t*)malloc(640*480*3);

    int i;
    for (i=0; i<2048; i++) {
        float v = i/2048.0;
        v = powf(v, 3)* 6;
        t_gamma[i] = v*6*256;
    }
#if 1
    if (freenect_init(&f_ctx, NULL) < 0) {
        printf("freenect_init() failed\n");
        return 1;
    }

    freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
    freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

    int nr_devices = freenect_num_devices (f_ctx);

    int user_device_number = 0;
    printf ("Number of devices found: %d || %d \n", nr_devices, argc);
    if(argc == 2){
    	/* we take the second parameter and make it int */
    	user_device_number = atoi(argv[1]);
    }
    printf ("Chosen device number %d \n", user_device_number);

    if (nr_devices < 1) {
        freenect_shutdown(f_ctx);
        return 1;
    }

    if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
        printf("Could not open device\n");
        freenect_shutdown(f_ctx);
        return 1;
    }

#else
    f_dev = get_device(0);
    f_ctx = get_context();
#endif
    res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
    if (res) {
        printf("pthread_create failed\n");
        freenect_shutdown(f_ctx);
        return 1;
    }

    // OS X requires GLUT to run on the main thread
    gl_threadfunc(argc, argv);

    return 0;
}
