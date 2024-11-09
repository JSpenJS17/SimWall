#ifndef BRIANS_BRAIN_H
#define BRIANS_BRAIN_H

typedef enum {
    DEAD,
    ALIVE,
    DYING
} BB_State;

int* bb_gen_next(const int* pattern, int board_width,int board_height);
int* bb_gen_random(int width, int height, int percent_alive);
void bb_add_life(int* pattern, int width, int height, int percent_alive);
#endif // BRIANS_BRAIN_H
