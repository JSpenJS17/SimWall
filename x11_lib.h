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

/* Generic POS struct */
struct POS {
    int x;
    int y;
};
typedef struct POS POS;

/* Function prototypes */
void x11_cleanup();
void fill_cell(int x, int y, int size);
void fill_circle(int x, int y, int size);
void color(RGB rgb);
int rgb_to_int(RGB rgb);
Display* window_setup(RGB bg_color);
int screen_width();
int screen_height();
void raise_window();
void lower_window();
void flush();
bool check_for_keybind(char* key);
bool wait_for_keybind(char* key);
void setup_keybind(char* key);
Window* get_window();
void focus_window();
void unfocus_window();
POS get_mouse_pos();
bool is_lmb_pressed();
void display_image(char* path, int x, int y);

#endif