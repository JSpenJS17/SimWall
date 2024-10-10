#ifndef X11_LIB_H
#define X11_LIB_H

#define CELL_SIZE 25

/* Generic RGB struct */
struct RGB {
    unsigned short r;
    unsigned short g;
    unsigned short b;
};
typedef struct RGB RGB;

/* typedefs for my convenience */
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

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