// game_of_life.h
#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <stdio.h>
#include <stdlib.h>

int* read_start_pattern(const char* filename, int max_width, int max_height);
int* gol_gen_next(const int* pattern, int width, int height);
int gol_count_live_neighbors(const int* pattern, int width, int height, int cell_index);
void print_pattern(const int* pattern, int width, int height);
int* gol_gen_random(int width, int height, int percent_alive);
void gol_add_life(int* pattern, int width, int height, int percent_alive);

#endif /* GAME_OF_LIFE_H */
