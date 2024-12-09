/*
Name:seeds.c

Authors:
    - Holden Vail

Creation Date: 10-07-2024
Description: This program reads a Seeds board state and generates some number of future board states according
            to the rules of Seeds
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "seeds.h"

// Commented out to keep purely library
// int main(){

//     char* filename = "start_pattern.txt";
//     int generations = 10000000;

//     int screen_ratio_width = 16;
//     int screen_ratio_height = 9;

//     int resolution_scaling_factor = 3;

//     int board_width = screen_ratio_width * resolution_scaling_factor;
//     int board_height = screen_ratio_height * resolution_scaling_factor;

//     bool* start_pattern = read_start_pattern(filename, board_width, board_height);
//     randomize_pattern(start_pattern, board_width, board_height);
//     if (start_pattern == NULL) {
//         return EXIT_FAILURE;
//     }

//     // run main loop
//     bool* current_pattern = start_pattern;
//     for (int i = 0; i < generations; i++) {
//         print_pattern(current_pattern, board_width, board_height);
//         usleep(100000);
//         bool* next_pattern = generate_next_pattern(current_pattern, board_width, board_height);
//         free(current_pattern);
//         current_pattern = next_pattern;
//     }

//     return 0;
// }


int* seeds_read_start_pattern(char* filename, int max_width, int max_height) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // First pass to determine pattern dimensions
    int pattern_width = 0;
    int pattern_height = 0;
    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        pattern_height++;
        int line_length = strlen(line);
        if (line[line_length - 1] == '\n') {
            line_length--;
        }
        if (line_length > pattern_width) {
            pattern_width = line_length;
        }
    }

    // Check if the pattern is larger than the max dimensions
    if (pattern_width > max_width || pattern_height > max_height) {
        fprintf(stderr, "Pattern is larger than the max dimensions (%d x %d)\n", max_width, max_height);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the full board
    int* board = (int*)calloc(max_width * max_height, sizeof(int));
    if (board == NULL) {
        perror("Failed to allocate memory for board");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Calculate starting position to center the pattern
    int start_x = (max_width - pattern_width) / 2;
    int start_y = (max_height - pattern_height) / 2;

    // Second pass to read the pattern and place it on the board
    fseek(file, 0, SEEK_SET);
    int y = start_y;
    while (fgets(line, sizeof(line), file)) {
        int line_length = strlen(line);
        if (line[line_length - 1] == '\n') {
            line_length--;
        }
        for (int x = 0; x < line_length; x++) {
            board[(y * max_width) + (start_x + x)] = (line[x] == '1');
        }
        y++;
    }

    fclose(file);
    return board;
}


int* seeds_gen_next(int* pattern, int width, int height) {
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


int seeds_count_live_neighbors(int* pattern, int width, int height, int cell_index){
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
                if (pattern[neighbor_index] == 1) {
                    live_neighbors_count++;
                }
            }
        }
    }
    return live_neighbors_count;
}


// void print_pattern(int* pattern, int width, int height) {
//     system("clear");
//     for (int y = 0; y < height; y++) {
//         for (int x = 0; x < width; x++) {
//             printf("%c", pattern[y * width + x] ? 'O' : '-');
//         }
//         printf("\n");
//     }
//     printf("\n");
// }


int* seeds_gen_random(int width, int height, int percent_alive) {
    srand(time(NULL));
    int* pattern = (int*)malloc(width * height * sizeof(int));
    memset(pattern, 0, width * height * sizeof(int));
    // Create a block of 6x6 cells in the middle
    int start_x = width / 2 - 3;
    int start_y = height / 2 - 3;
    for (int y = start_y; y < start_y + 6; y++) {
        for (int x = start_x; x < start_x + 6; x++) {
            pattern[y*width + x] = (rand() % 100) < percent_alive;
        }
    }
    return pattern;
}

void seeds_add_life(int* pattern, int width, int height, int percent_alive) {
    // Airdrop some extra cells!!
    srand(time(NULL));
    for (int i = 0; i < width * height; i++) {
        if (pattern[i] == 0) {
            pattern[i] = (rand() % 100) < percent_alive;
        }
    }
}
