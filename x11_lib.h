#ifndef X11_LIB_H
#define X11_LIB_H

#define CELL_SIZE 25

unsigned char parse_args(int argc, char **argv);
void cleanup();
void fill_background();
void fill_cell(int x, int y);
void color(unsigned short r, unsigned short g, unsigned short b);
int window_setup(unsigned char flags);
int screen_width();
int screen_height();

#endif