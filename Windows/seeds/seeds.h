#ifndef SEEDS_H
#define SEEDS_H

// Function prototypes
int* seeds_read_start_pattern(char* filename, int max_width, int max_height);
int* seeds_gen_next(int* pattern, int width, int height);
int* seeds_gen_random(int width, int height, int percent_alive);
void seeds_add_life(int* pattern, int width, int height, int percent_alive);
int seeds_count_live_neighbors(int* pattern, int width, int height, int cell_index);

#endif