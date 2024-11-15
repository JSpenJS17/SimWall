/*
Name:game_of_life.c

Authors:
    - Holden Vail

Creation Date: 10-07-2024
Description: This program reads a Seeds board state and generates some number of future board states according
            to the rules of Seeds
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "seeds.h"

// Function prototypes

int seeds_count_live_neighbors(const int* pattern, int width, int height, int cell_index){
    int live_neighbors_count = 0;
    int cell_x = cell_index % width;
    int cell_y = cell_index / width;

    for (int delta_y = -1; delta_y <= 1; delta_y++) {
        for (int delta_x = -1; delta_x <= 1; delta_x++) {
            if (delta_x == 0 && delta_y == 0) continue; // Skip the cell itself

            int neighbor_x = (cell_x + delta_x + width) % width;
            int neighbor_y = (cell_y + delta_y + height) % height;

            int neighbor_index = neighbor_y * width + neighbor_x;
            if (pattern[neighbor_index]) {
                live_neighbors_count++;
            }
        }
    }
    return live_neighbors_count;
}


int* seeds_gen_next(const int* pattern, int width, int height) {
    int* next_pattern = (int*)malloc(width * height * sizeof(int));
    if (next_pattern == NULL) {
        perror("Failed to allocate memory for next pattern");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int cell_index = y * width + x;
            int live_neighbors = seeds_count_live_neighbors(pattern, width, height, cell_index);

            // Seeds rule: B2/S
            next_pattern[cell_index] = (!pattern[cell_index] && live_neighbors == 2);
        }
    }

    return next_pattern;
}
