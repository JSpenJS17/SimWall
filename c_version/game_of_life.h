#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

// Function prototypes
bool* read_start_pattern(char* filename, int max_width, int max_height);
bool* generate_next_pattern(bool* pattern, int width, int height);
int count_live_neighbors(bool* pattern, int width, int height, int cell_index);
void print_pattern(bool* pattern, int width, int height);
void randomize_pattern(bool* pattern, int width, int height);

#endif // GAME_OF_LIFE_H