#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

#include "../common/sample-common.h"

#define TAG "SAMPLE_MOUSE"
#define MOUSEFB "/dev/fb1"
#define GUIFB "/dev/fb0"
#define CANVAS_WIDTH (1920)
#define CANVAS_HEIGHT (1080)
#define WHEEL_UP    0x10
#define WHEEL_DOWN  0x08
#define BUTTON_L    (1 << 0)
#define BUTTON_M    (1 << 1)
#define BUTTON_R	(1 << 2)
#define SCALE       3
#define THRESH      5

static int cursor_fd;
static unsigned char IMPS2_Param [] = {243,200,243,100,243,80};
static int    xpos;        /* current x position of mouse */
static int    ypos;        /* current y position of mouse */
static int    minx;        /* minimum allowed x position */
static int    maxx;        /* maximum allowed x position */
static int    miny;        /* minimum allowed y position */
static int    maxy;        /* maximum allowed y position */
static int    buttons;     /* current state of buttons */
static int    scale;       /* acceleration scale factor */
static int    thresh;      /* acceleration threshhold */

//A=1 不透明 A=0 透明
const unsigned char cursor_image[2048] = { /* 0X00,0X10,0X20,0X00,0X20,0X00,0X00,0X1B, */
0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0X00,0X80,0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0XFF,0XFF,0X00,0X80,0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0XFF,0XFF,0X00,0X80,0X00,0X80,0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0XFF,0XFF,0XFF,0XFF,0X00,0X80,0X00,0X80,0X00,0X80,0X00,0X80,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0XFF,0XFF,0XFF,0XFF,0X00,0X80,0X00,0X80,0X00,0X80,0X00,0X80,0X00,0X80,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X80,0X00,0X80,0X00,0X80,0X00,0X80,
0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X80,0X00,0X80,0X00,0X80,0X00,0X80,
0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0XFF,0XFF,0XFF,0XFF,0X00,0X80,0X00,0X80,0X00,0X80,0X00,0X80,0X00,0X80,
0X00,0X80,0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0XFF,0XFF,0X00,0X80,0X00,0X80,0XFF,0XFF,0X00,0X80,0X00,0X80,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0X00,0X80,0X00,0X80,0XFC,0X00,0X00,0X80,0XFF,0XFF,0X00,0X80,0X00,0X80,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0X00,0X80,0XFF,0XFF,0X00,0X80,0X00,0X80,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0X00,0X80,0XFF,0XFF,0X00,0X80,
0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0X00,0X80,0XFF,0XFF,0X00,0X80,
0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0X00,0X80,0XFF,0XFF,
0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0X00,0X80,0X00,0X80,
0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0X00,0X80,
0X00,0X80,0X00,0X80,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,

0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,
};

static int IMPS2_Open (void)
{
    cursor_fd = open ("/dev/input/mice", O_RDWR | O_NONBLOCK);
    if (cursor_fd < 0) {
        cursor_fd = open ("/dev/input/mice", O_RDONLY | O_NONBLOCK);
        if (cursor_fd < 0)
            return -1;
    }
    else {
        if (write (cursor_fd, IMPS2_Param, sizeof (IMPS2_Param)) < sizeof (IMPS2_Param)) {
            return -1;
        }
    }
    return cursor_fd;
}

static void IMPS2_Close (void)
{
    if (cursor_fd > 0)
        close (cursor_fd);

    cursor_fd = -1;
}

static int IMPS2_GetButtonInfo(void)
{
    return BUTTON_L | BUTTON_M | BUTTON_R | WHEEL_UP | WHEEL_DOWN;
}

static void IMPS2_GetDefaultAccel(int *pscale, int *pthresh)
{
    *pscale = SCALE;
    *pthresh = THRESH;
}

static int IMPS2_Read (int *dx, int *dy, int *dz, int *bp)
{
    static unsigned char buf[5];
    static int buttons[4] = { 0, BUTTON_L, BUTTON_R, BUTTON_L | BUTTON_R};
    static int nbytes;
    int n;

    while ((n = read (cursor_fd, &buf [nbytes], 4 - nbytes))) {
        if (n < 0) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }

        nbytes += n;

        if (nbytes == 4) {
            int wheel;

            if ((buf[0] & 0xc0) != 0) {
                buf[0] = buf[1];
                buf[1] = buf[2];
                buf[2] = buf[3];
                nbytes = 3;

                return -1;
            }
            *bp = buttons[(buf[0] & 0x03)];
            *dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
            *dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];

            if ((wheel = buf[3]) != 0) {
                if(wheel > 0x7f) {
                    *bp = WHEEL_UP;
                }
                else {
                    *bp = WHEEL_DOWN;
                }
            }

            *dz = 0;
            nbytes = 0;
            return 1;
		}
	}
	return 0;
}

static int cursor_update(void)
{
	int dx,dy,dz;
	int r;
	int sign;
	dx = dy = 0;
	r = IMPS2_Read (&dx, &dy, &dz, &buttons);
	if (r <= 0)
		return 0;
	sign = 1;
	if (dx < 0) {
		sign = -1;
		dx = -dx;
	}
	if (dx > thresh)
		dx = thresh + (dx - thresh) * scale;
	dx *= sign;
	xpos += dx;
	if( xpos < minx )
		xpos = minx;
	if( xpos > maxx )
		xpos = maxx;
	sign = 1;
	if (dy < 0) {
		sign = -1;
		dy = -dy;
	}
	if (dy > thresh)
		dy = thresh + (dy - thresh) * scale;
	dy *= sign;
	ypos += dy;
	if ( ypos < miny )
		ypos = miny;
	if ( ypos > maxy )
		ypos = maxy;
	return 1;
}

static int init_cursor(void)
{
	if(IMPS2_Open() < 0){
		perror("open cursor failed!\n");
		return -1;
	}
	xpos = 0;
	ypos = 0;
	buttons = 0;
	minx = 0;
	miny = 0;
	maxx = CANVAS_WIDTH;
	maxy = CANVAS_HEIGHT;
	scale = SCALE;
	thresh = THRESH;
	return 0;
}

static void deinit_cursor(void)
{
	IMPS2_Close();
}

static int wait_cursor_event(struct timeval *timeout)
{
	int ret = 0;
	int maxfd = cursor_fd;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(cursor_fd,&readfds);
	ret = select(maxfd+1,&readfds,NULL,NULL,timeout);
	if(ret < 0){
		perror("cursor is disconnected!\n");
		return -1;
	}else if (ret > 0){
		if(FD_ISSET(cursor_fd,&readfds)){
			FD_CLR(cursor_fd, &readfds);
			cursor_update();
			return 1;
		}
	}
	return 1;
}

static int get_cursor_info(int* x, int* y, int* button)
{
	*x = xpos;
	*y = ypos;
	if(*x < 0) *x = 0;
	if(*y < 0) *y = 0;
	if(*x > CANVAS_WIDTH-1) *x = CANVAS_WIDTH-1;
	if(*y > CANVAS_HEIGHT-1) *y = CANVAS_HEIGHT-1;
	*button = buttons;
	return 0;
}

void *display_gui_thread(void *arg)
{
	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo var_info;
	unsigned char *pShowScreen;
	int ret = 0;
	int fd;
	IMP_BOOL bShow = IMP_TRUE;
	int i = 0;
	IMP_SIZE_S *stDisplaySize = (IMP_SIZE_S *)arg;

	fd = open(GUIFB, O_RDWR);
	if (fd < 0 ) {
		printf("%s open error!\n",GUIFB);
		return NULL;
	}

	/* get framebuffer's var_info */
	if ((ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		printf("FBIOGET_VSCREENINFO failed\n");
		return NULL;
	}
	printf("\ngui var info:\n");
	printf("[xres,yres]=[%d,%d]\n",var_info.xres,var_info.yres);
	printf("[xres_virtual,yres_virtual]=[%d,%d]\n",var_info.xres_virtual,var_info.yres_virtual);
	printf("[bits_per_pixel]=[%d]\n",var_info.bits_per_pixel);
	printf("R[%d,%d,%d],G[%d,%d,%d],B[%d,%d,%d],A[%d,%d,%d]\n",
			var_info.red.offset,var_info.red.length,var_info.red.msb_right,
			var_info.green.offset,var_info.green.length,var_info.green.msb_right,
			var_info.blue.offset,var_info.blue.length,var_info.blue.msb_right,
			var_info.transp.offset,var_info.transp.length,var_info.transp.msb_right);

	var_info.activate = FB_ACTIVATE_NOW;
	var_info.xres = ALIGN_UP(stDisplaySize->u32Width,4);
	var_info.yres = stDisplaySize->u32Height;
	var_info.xres_virtual = ALIGN_UP(stDisplaySize->u32Width,4);
	var_info.yres_virtual = stDisplaySize->u32Height * 2;
	var_info.bits_per_pixel = 16;

	/* put framebuffer's var_info */
	if ((ret = ioctl(fd, FBIOPUT_VSCREENINFO, &var_info)) < 0) {
		printf("FBIOGET_VSCREENINFO failed\n");
		return NULL;
	}

	if ((ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		printf("FBIOGET_VSCREENINFO failed\n");
		return NULL;
	}

	/* get framebuffer's fix_info */
	if ((ret = ioctl(fd, FBIOGET_FSCREENINFO, &fix_info)) < 0) {
		printf("FBIOGET_FSCREENINFO failed\n");
		return NULL;
	}

	printf("\ngui var info:\n");
	printf("[xres,yres]=[%d,%d]\n",var_info.xres,var_info.yres);
	printf("[xres_virtual,yres_virtual]=[%d,%d]\n",var_info.xres_virtual,var_info.yres_virtual);
	printf("[bits_per_pixel]=[%d]\n",var_info.bits_per_pixel);
	printf("R[%d,%d,%d],G[%d,%d,%d],B[%d,%d,%d],A[%d,%d,%d]\n",
			var_info.red.offset,var_info.red.length,var_info.red.msb_right,
			var_info.green.offset,var_info.green.length,var_info.green.msb_right,
			var_info.blue.offset,var_info.blue.length,var_info.blue.msb_right,
			var_info.transp.offset,var_info.transp.length,var_info.transp.msb_right);

	printf("\ngui fix info:\n");
	printf("[line_length,smem_len] = [%d,%d]\n", fix_info.line_length,fix_info.smem_len);
	printf("[smem_start] = [0x%08lx]\n",fix_info.smem_start);
	printf("[xpanstep,ypanstep] = [%d,%d]\n",fix_info.xpanstep,fix_info.ypanstep);

	pShowScreen = mmap(0, fix_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(!pShowScreen) {
		printf("mmap failed!\n");
		return NULL;
	}
	memset(pShowScreen, 0x0, fix_info.smem_len);
	ioctl(fd, FBIOPUT_SHOW, &bShow);

	short colors[] = {0x7C00, 0x001F};
	for(i=0; i < var_info.xres*var_info.yres; i++){
		memcpy(pShowScreen + i*(var_info.bits_per_pixel >> 3), &colors[0], 2);
	}

	printf("displaying red.....\n");
	sleep(10);
	FB_COLORKEY_S stColorkey;
	stColorkey.s32KeyEnable = 1;
	stColorkey.u32KeyColor = colors[0];
	ioctl(fd, FBIOPUT_COLORKEY, &stColorkey);
	printf("enable colorkey red.....\n");
	sleep(10);
	stColorkey.s32KeyEnable = 0;
	stColorkey.u32KeyColor = colors[0];
	ioctl(fd, FBIOPUT_COLORKEY, &stColorkey);
	printf("disable colorkey red.....\n");
	sleep(10);
	FB_ALPHA_S stAlpha;
	stAlpha.enAlphaMode = ALPHA_PIXEL;
	stAlpha.u8Alpha0 = 255;
	stAlpha.u8Alpha1 = 0;
	stAlpha.u8GlobalAlpha = 128;
	ioctl(fd, FBIOPUT_ALPHA, &stAlpha);
	printf("red is set to full transparency.....\n");
	sleep(10);
	munmap(pShowScreen, fix_info.smem_len);
	close(fd);
	return NULL;
}

void *display_cursor_thread(void *parg)
{
	IMP_S32 ret = IMP_SUCCESS;
	IMP_S32 fd = 0;
	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo var_info;
	unsigned char *pBufferAddr;
	IMP_BOOL bShow = IMP_FALSE;
	FB_POINT_S stPoint;
	IMP_S32 s32CursorButton;
	struct timeval timeout;

	init_cursor();
	fd = open(MOUSEFB, O_RDWR);
	if (fd < 0) {
		perror("FB1DEV open error!\n");
		return NULL;
	}

	if ((ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		perror("FBIOGET_VSCREENINFO failed");
		goto exit;
	}

	var_info.activate = FB_ACTIVATE_FORCE;
	var_info.xres = 32;
	var_info.yres = 32;
	var_info.bits_per_pixel = 16;
	//A
	var_info.transp.offset = 15;
	var_info.transp.length = 1;
	var_info.transp.msb_right = 0;
	//R
	var_info.red.offset = 10;
	var_info.red.length = 5;
	var_info.red.msb_right = 0;
	//G
	var_info.green.offset = 5;
	var_info.green.length = 5;
	var_info.green.msb_right = 0;
	//B
	var_info.blue.offset = 0;
	var_info.blue.length = 5;
	var_info.blue.msb_right = 0;

	if ((ret = ioctl(fd, FBIOPUT_VSCREENINFO, &var_info)) < 0) {
		perror("FBIOPUT_VSCREENINFO failed");
		goto exit;
	}

	if ((ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		perror("FBIOGET_VSCREENINFO failed");
		goto exit;
	}

	if ((ret = ioctl(fd, FBIOGET_FSCREENINFO, &fix_info)) < 0) {
		perror("FBIOGET_FSCREENINFO failed");
		goto exit;
	}
	/*printf("[xres,yres]=[%d,%d]\n",var_info.xres,var_info.yres);
	printf("[xres_virtual,yres_virtual]=[%d,%d]\n",var_info.xres_virtual,var_info.yres_virtual);
	printf("[bits_per_pixel]=[%d]\n",var_info.bits_per_pixel);*/

	pBufferAddr = mmap(0,fix_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(!pBufferAddr) {
		perror("mmap failed");
		goto exit;
	}

	memset(pBufferAddr, 0xff, fix_info.smem_len);
	memcpy(pBufferAddr, cursor_image, 2048);

	bShow = IMP_TRUE;
	ioctl(fd, FBIOPUT_SHOW, &bShow);

#if 0	//使用colorkey或者pixel alpha让不需要的区域透明
	FB_COLORKEY_S stColorKey;
	stColorKey.s32KeyEnable = 1;
	stColorKey.u32KeyColor = 0x00FC;
	ioctl(fd, FBIOPUT_COLORKEY, &stColorKey);
#else
	FB_ALPHA_S stAlpha;
	stAlpha.enAlphaMode = ALPHA_PIXEL;
	stAlpha.u8Alpha0 = 255;
	stAlpha.u8Alpha1 = 0;
	stAlpha.u8GlobalAlpha = 128;
	ioctl(fd, FBIOPUT_ALPHA, &stAlpha);
#endif

	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;
	while(1) {
		ret = wait_cursor_event(&timeout);
		if(ret < 0){
			perror("wait_cursor_event failed\n");
			break;
		}

		get_cursor_info(&stPoint.s32X, &stPoint.s32Y, &s32CursorButton);
		//printf("cursor=(%d,%d)\n",stPoint.s32X, stPoint.s32Y);
		if(ioctl(fd, FBIOPUT_SCREEN_ORIGIN, &stPoint) < 0){
			break;
		}
	}

	munmap(pBufferAddr, fix_info.smem_len);
	deinit_cursor();
exit:
	close(fd);
	return NULL;
}

int main(int argc,char *argv[])
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32VoMod;
	IMP_S32 s32VoChn;
	IMP_S32 s32VdecDev;
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVoCell = {0};
	SAMPLE_VDEC_THREAD_S stThreadParam;
	VDEC_DEV_ATTR_S stVdecDevAttr;
	VDEC_CHN_ATTR_S stVdecChnAttr;
	VO_PUB_ATTR_S stVoPubAttr;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
	VO_CHN_ATTR_S stVoChnAttr;
	pthread_t sendTid;
	pthread_t cursor_thread;
	pthread_t gui_thread;

    /************************************************
    step1:  init system
    *************************************************/
	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_system_init failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

    /************************************************
    step2:  bind vdec and vo
    *************************************************/
	s32VdecDev = 0;
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = s32VdecDev;
	stVdecCell.u32ChnId = 0;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = SAMPLE_VO_LATER0;
	stVoCell.u32ChnId = 0;
	s32Ret = IMP_System_Bind(&stVdecCell, &stVoCell);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind failed! ret = 0x%08x\n", s32Ret);
		goto END0;
	}
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = s32VdecDev;
	stVdecCell.u32ChnId = 1;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = SAMPLE_VO_LATER1;
	stVoCell.u32ChnId = 0;
	s32Ret = IMP_System_Bind(&stVdecCell, &stVoCell);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind failed! ret = 0x%08x\n", s32Ret);
		goto END1;
	}

    /************************************************
    step3:  start vo
    *************************************************/
	s32VoMod = SAMPLE_VO_MODULE;
	IMP_VO_Disable(s32VoMod);
	stVoPubAttr.u32BgColor = 0xffffffff;
	stVoPubAttr.stCanvasSize.u32Width = CANVAS_WIDTH;
	stVoPubAttr.stCanvasSize.u32Height = CANVAS_HEIGHT;
	stVoPubAttr.enIntfType = VO_INTF_HDMI;
	stVoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
	s32Ret = sample_vo_start_module(s32VoMod,&stVoPubAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_module failed! ret = 0x%08x\n", s32Ret);
		goto END2;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if(s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG, "sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto END3;
	}

	s32Ret = sample_vo_getwh(stVoPubAttr.enIntfSync, &stVoLayerAttr.stDispRect.u32Width,
			&stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_getwh failed! ret = %d\n", s32Ret);
		goto END4;
	}

	stVoLayerAttr.enPixFmt = PIX_FMT_NV12;
	stVoLayerAttr.stDispRect.s32X = ORIGINAL_X;
	stVoLayerAttr.stDispRect.s32Y = ORIGINAL_Y;
	stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
	stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
	s32Ret = sample_vo_start_layer(SAMPLE_VO_LATER0, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END5;
	}
	s32Ret = sample_vo_start_layer(SAMPLE_VO_LATER1, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END6;
	}

	s32VoChn = SAMPLE_VO_CHN0;
	stVoChnAttr.u32Priority = 0;
	stVoChnAttr.bDeflicker = IMP_FALSE;
	stVoChnAttr.stRect.s32X = ORIGINAL_X;
	stVoChnAttr.stRect.s32Y = ORIGINAL_Y;
	stVoChnAttr.stRect.u32Width = 1920;
	stVoChnAttr.stRect.u32Height = 1080;
	s32Ret = sample_vo_start_chn(SAMPLE_VO_LATER0,s32VoChn,&stVoChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END7;
	}

	stVoChnAttr.stRect.u32Width = 400;
	stVoChnAttr.stRect.u32Height = 240;
	stVoChnAttr.stRect.s32X = ORIGINAL_X;
	stVoChnAttr.stRect.s32Y = stVoLayerAttr.stImageSize.u32Height - stVoChnAttr.stRect.u32Height;
	s32Ret = sample_vo_start_chn(SAMPLE_VO_LATER1,s32VoChn,&stVoChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END8;
	}

    /************************************************
    step4:  start vdec
    *************************************************/
	stVdecDevAttr.enType = PT_H265;
	stVdecDevAttr.enMode = VIDEO_MODE_FRAME;
	stVdecDevAttr.u32PicWidth = 1920;
	stVdecDevAttr.u32PicHeight = 1080;
	stVdecDevAttr.u32StreamBufSize = 1920*1080*3/4;
	stVdecDevAttr.u32RefFrameNum = 2;
	s32Ret = sample_vdec_start_dev(s32VdecDev, &stVdecDevAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
		goto END9;
	}

	stVdecChnAttr.enOutputFormat = PIX_FMT_NV12;
	stVdecChnAttr.u32OutputWidth = 1920;
	stVdecChnAttr.u32OutputHeight = 1080;
	stVdecChnAttr.bEnableCrop = IMP_FALSE;
	stVdecChnAttr.u32FrameBufCnt = 3;
	s32Ret = sample_vdec_start_chn(s32VdecDev,0,&stVdecChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
		goto END10;
	}

	stVdecChnAttr.enOutputFormat = PIX_FMT_NV12;
	stVdecChnAttr.u32OutputWidth = 400;
	stVdecChnAttr.u32OutputHeight = 240;
	stVdecChnAttr.bEnableCrop = IMP_FALSE;
	stVdecChnAttr.u32FrameBufCnt = 3;
	s32Ret = sample_vdec_start_chn(s32VdecDev,1,&stVdecChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
		goto END11;
	}

	s32Ret = sample_vdec_start_recv_stream(s32VdecDev);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_recv_stream failed! ret = 0x%08x\n", s32Ret);
		goto END12;
	}

    /************************************************
    step5:  send stream to vdec
    *************************************************/
	sample_vdec_thread_param(1,&stThreadParam,&stVdecDevAttr,"1920x1080.hevc");
	stThreadParam.bLoopPlayback = IMP_TRUE;
	s32Ret = sample_vdec_start_send_stream(1,&stThreadParam, &sendTid);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_send_stream failed! ret = 0x%08x\n", s32Ret);
		goto END13;
	}

    /************************************************
    step6:  start fb
    *************************************************/
	pthread_create(&cursor_thread, NULL, display_cursor_thread, NULL);

	pthread_create(&gui_thread,NULL,display_gui_thread,&stVoPubAttr.stCanvasSize);

	pthread_join(gui_thread, NULL);

	pthread_join(cursor_thread, NULL);

	sample_vdec_stop_send_stream(1,&stThreadParam, &sendTid);
END13:
	sample_vdec_stop_recv_stream(0);
END12:
	sample_vdec_stop_chn(0,1);
END11:
	sample_vdec_stop_chn(0,0);
END10:
	sample_vdec_stop_dev(0);
END9:
	sample_vo_stop_chn(SAMPLE_VO_LATER1,s32VoChn);
END8:
	sample_vo_stop_chn(SAMPLE_VO_LATER0,s32VoChn);
END7:
	sample_vo_stop_layer(SAMPLE_VO_LATER1);
END6:
	sample_vo_stop_layer(SAMPLE_VO_LATER0);
END5:
END4:
	sample_vo_hdmi_stop();
END3:
	sample_vo_stop_module(s32VoMod);
END2:
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = 0;
	stVdecCell.u32ChnId = 1;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = SAMPLE_VO_LATER1;
	stVoCell.u32ChnId = 0;
	IMP_System_UnBind(&stVdecCell, &stVoCell);
END1:
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = 0;
	stVdecCell.u32ChnId = 0;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = SAMPLE_VO_LATER0;
	stVoCell.u32ChnId = 0;
	IMP_System_UnBind(&stVdecCell, &stVoCell);
END0:
	sample_system_exit();
	return IMP_SUCCESS;
}

