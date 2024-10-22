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
void cleanup();
void fill_cell(int x, int y, int size);
void fill_circle(int x, int y, int size);
void color(RGB rgb);
int rgb_to_int(RGB rgb);
HWND window_setup(RGB bg_color);
int screen_width();
int screen_height();
void raise_window();
void lower_window();
void flush();
bool check_for_keybind(char* key);
bool wait_for_keybind(char* key);
HWND get_window();
void focus_window();
void unfocus_window();
POINT get_mouse_pos();
bool is_lmb_pressed();
HWND GetDesktopWorker();


#endif