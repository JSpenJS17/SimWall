#ifndef X11_LIB_H
#define X11_LIB_H

#define CELL_SIZE 25

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
void fill_cell(int x, int y);
void color(ushort r, ushort g, ushort b);
void color_rgb(RGB rgb);
int window_setup();
int screen_width();
int screen_height();

#endif