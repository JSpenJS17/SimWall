#ifndef WINDOWS_LIB_H
#define WINDOWS_LIB_H



#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>

/* typedefs for my convenience */
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

#ifndef RGBA_S
#define RGBA_S
/* Generic RGBA struct */
typedef struct RGBA {
    unsigned short r;
    unsigned short g;
    unsigned short b;
    unsigned short a;
} RGBA;
#endif

/* Generic POS struct */
struct POS {
    int x;
    int y;
};
typedef struct POS POS;

/* Function prototypes */
void cleanup();
void fill_cell(int x, int y, size_t size);
void fill_circle(int x, int y, size_t size);
void color(RGBA rgba);
HWND window_setup(RGBA bg_color);
int screen_width();
int screen_height();
POINT get_mouse_pos();
bool is_lmb_pressed();
HWND GetDesktopWorker();
extern bool paused;
extern bool add_mode;
extern bool clear;


#endif