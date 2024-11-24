#ifndef SEEDS_H
#define SEEDS_H

// Function prototypes
int seeds_count_live_neighbors(const int* pattern, int width, int height, int cell_index);
int* seeds_gen_next(const int* pattern, int width, int height);
int count_live_neighbors(const int* pattern, int width, int height, int cell_index);

#endif
