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


int count_live_neighbors(int* pattern, int width, int height, int cell_index) {
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
                if (pattern[neighbor_index] == 2) {
                    live_neighbors_count++;
                }
            }
        }
    }
    return live_neighbors_count;
}


// Patterns should only contain 0s (dead), 1s(dying), and 2s (alive)
int* generate_next_pattern(int* pattern, int board_width,int board_height){
    int* next_pattern = (int*)malloc(board_width * board_height * sizeof(int));
    if (next_pattern == NULL) {
        perror("Failed to allocate memory for next pattern");
        exit(EXIT_FAILURE);
    }
    
    for (int y = 0; y < board_height; y++) {
        for (int x = 0; x < board_width; x++) {
            int cell_index = y * board_width + x;
            int live_neighbors = count_live_neighbors(pattern, board_width, board_height, cell_index);
            int cell_value = pattern[cell_index];

            if (cell_value == 0 && live_neighbors == 2) {
                next_pattern[cell_index] = 2;
            } else if (cell_value == 1) {
                next_pattern[cell_index] = 0;
            } else if (cell_value == 2) {
                next_pattern[cell_index] = 1;
            } else {
                next_pattern[cell_index] = cell_value;
            }
        }
    }
    return next_pattern;
}


int* generate_random_pattern(int width, int height) {
    int* pattern = (int*)malloc(width * height * sizeof(int));
    if (pattern == NULL) {
        perror("Failed to allocate memory for random pattern");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    for (int i = 0; i < width * height; i++) {
        pattern[i] = (rand() % 2 == 0) ? 2 : 0;
    }

    return pattern;
}

void print_board(int* pattern, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int cell_value = pattern[y * width + x];
            if (cell_value == 0) {
                printf("-");
            } else if (cell_value == 1) {
                printf("*");
            } else if (cell_value == 2) {
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
        if (pattern[i] == 2) {
            live_cells++;
        }
    }
    float life = (float)live_cells / total_cells;
    return life;
}

void add_life(int* pattern, int width, int height){
    srand(time(NULL));
    for (int i = 0; i < width * height; i++){
        if (pattern[i] == 0){
            if (rand() % 10 == 0) {
                pattern[i] = 2;
            }
        }
    }
}

int main(){
    int board_width = 100;
    int board_height = 30;

    int generations = 1000;

    int* current_pattern = generate_random_pattern(board_width, board_height);
    
    for  (int i = 0; i < generations; i++){
        system("clear");
        printf("\n");
        print_board(current_pattern, board_width, board_height);
        current_pattern = generate_next_pattern(current_pattern, board_width, board_height);
        float life = measure_life(current_pattern, board_width, board_height);
        printf("Life: %f", life);
        if (life < 0.01){
            add_life(current_pattern, board_width, board_height);
        }
        usleep(250000);
    }
    return 0;
}
