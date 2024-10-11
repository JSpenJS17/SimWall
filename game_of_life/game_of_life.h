#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

// Function prototypes
int* read_start_pattern(char* filename, int max_width, int max_height);
int* gol_gen_next(int* pattern, int width, int height);
int* gol_gen_random(int width, int height, int percent_alive);
void gol_add_life(int* pattern, int width, int height, int percent_alive);
int gol_count_live_neighbors(int* pattern, int width, int height, int cell_index);

#endif // GAME_OF_LIFE_H