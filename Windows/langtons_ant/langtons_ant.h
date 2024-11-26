#ifndef LANGTONS_ANT_H
#define LANGTONS_ANT_H

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    NUM_DIRS
} Direction;

#ifndef RGBA_S
#define RGBA_S
typedef struct RGBA {
    unsigned short r;
    unsigned short g;
    unsigned short b;
    unsigned short a;
} RGBA;
#endif

// Struct to store ant information
typedef struct {
    int x;
    int y;
    Direction direction; // 0:UP, 1:RIGHT, 2:DOWN, 3:LEFT
    RGBA color;
} Ant;

int* ant_gen_next(int* grid, int width, int height);
void ant_add_life(int* pattern, int width, int height, int percent_alive);
int* ant_gen_random(int width, int height, int percent_alive);
void init_ants(Ant* inp_ants, int num_ants, char* ruleset);

#endif