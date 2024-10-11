/*
Name:game_of_life.c

Authors:
    - Holden Vail

Creation Date: 10-10-2024
Description: This program reads a Brain's Brain board state and generates some number of future board states according
            to the rules of Brain's Brain
*/


#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "brians_brain.h"

int bb_count_live_neighbors(int* pattern, int width, int height, int cell_index) {
    int live_neighbors_count = 0;
    int cell_x = cell_index % width;
    int cell_y = cell_index / width;

    for (int delta_y = -1; delta_y <= 1; delta_y++) {
        for (int delta_x = -1; delta_x <= 1; delta_x++) {
            if (delta_x == 0 && delta_y == 0) continue; // Skip the cell itself

            int neighbor_x = cell_x + delta_x;
            int neighbor_y = cell_y + delta_y;

            if (neighbor_x >= 0 && neighbor_x < width && neighbor_y >= 0 && neighbor_y < height) {
                int neighbor_index = neighbor_y * width + neighbor_x;
                if (pattern[neighbor_index] == ALIVE) {
                    live_neighbors_count++;
                }
            }
        }
    }
    return live_neighbors_count;
}


// Patterns should only contain 0s (dead), 1s(dying), and 2s (alive)
int* bb_gen_next(int* pattern, int board_width,int board_height){
    int* next_pattern = (int*)malloc(board_width * board_height * sizeof(int));
    if (next_pattern == NULL) {
        perror("Failed to allocate memory for next pattern");
        exit(EXIT_FAILURE);
    }
    
    for (int y = 0; y < board_height; y++) {
        for (int x = 0; x < board_width; x++) {
            int cell_index = y * board_width + x;
            int live_neighbors = bb_count_live_neighbors(pattern, board_width, board_height, cell_index);
            int cell_value = pattern[cell_index];

            if (cell_value == DEAD && live_neighbors == 2) {
                next_pattern[cell_index] = ALIVE;
            } else if (cell_value == DYING) {
                next_pattern[cell_index] = DEAD;
            } else if (cell_value == ALIVE) {
                next_pattern[cell_index] = DYING;
            } else {
                next_pattern[cell_index] = cell_value;
            }
        }
    }
    return next_pattern;
}


int* bb_gen_random(int width, int height, int percent_alive){
    int* pattern = (int*)malloc(width * height * sizeof(int));
    if (pattern == NULL) {
        perror("Failed to allocate memory for random pattern");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    for (int i = 0; i < width * height; i++) {
        if (rand() % 100 < percent_alive) {
            pattern[i] = ALIVE;
        } else {
            pattern[i] = DEAD;
        }
    }

    return pattern;
}

void print_board(int* pattern, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int cell_value = pattern[y * width + x];
            if (cell_value == DEAD) {
                printf("-");
            } else if (cell_value == DYING) {
                printf("*");
            } else if (cell_value == ALIVE) {
                printf("0");
            }
        }
        printf("\n");
    }
}

float measure_life(int* pattern, int board_width, int board_height){
    int total_cells = board_width * board_height;
    int live_cells = 0;

    for (int i = 0; i < total_cells; i++) {
        if (pattern[i] == ALIVE) {
            live_cells++;
        }
    }
    float life = (float)live_cells / total_cells;
    return life;
}

void bb_add_life(int* pattern, int width, int height, int percent_alive){
    srand(time(NULL));
    for (int i = 0; i < width * height; i++){
        if (pattern[i] == DEAD){
            if (rand() % 100 < percent_alive){
                pattern[i] = ALIVE;
            }
        }
    }
}

// Same as game of life, uncomment if you want to run this as a standalone program
// int main(){
//     int board_width = 100;
//     int board_height = 30;

//     int generations = 1000;

//     int* current_pattern = bb_gen_random(board_width, board_height);
    
//     for  (int i = 0; i < generations; i++){
//         system("clear");
//         printf("\n");
//         print_board(current_pattern, board_width, board_height);
//         current_pattern = bb_gen_next(current_pattern, board_width, board_height);
//         float life = measure_life(current_pattern, board_width, board_height);
//         printf("Life: %f", life);
//         if (life < 0.01){
//             bb_add_life(current_pattern, board_width, board_height);
//         }
//         usleep(250000);
//     }
//     return 0;
// }
