#ifndef BRIANS_BRAIN_H
#define BRIANS_BRAIN_H

typedef enum {
    DEAD,
    DYING,
    ALIVE
} State;

int* generate_next_pattern(int* pattern, int board_width,int board_height);
int* generate_random_pattern(int width, int height);
void add_life(int* pattern, int width, int height);
#endif // BRIANS_BRAIN_H