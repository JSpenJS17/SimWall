#ifndef X11_LIB_H
#define X11_LIB_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xproto.h>

#include <X11/extensions/shape.h>
#include <X11/extensions/Xrender.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

/* typedefs for my convenience */
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

/* Generic RGB struct */
struct RGB {
    ushort r;
    ushort g;
    ushort b;
};
typedef struct RGB RGB;

/* Function prototypes */
void x11_cleanup();
void fill_background();
void fill_cell(int x, int y, int size);
void fill_circle(int x, int y, int size);
void color(ushort r, ushort g, ushort b);
void color_rgb(RGB rgb);
Display* window_setup();
int screen_width();
int screen_height();

#endif