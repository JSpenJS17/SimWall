/*
Name: game_of_life.c

Authors:
    - Holden Vail

Creation Date: 10-07-2024
Description: This program reads a Game of Life board state and generates some number of future board states according
            to the rules of Conway's Game of Life
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include "game_of_life.h"

// commented out to keep this file as a library
    // can uncomment for testing purposes if needed
    // since I renamed some of the functions you'll have to sort it out though
// int main(int argc, char* argv[]){
//     if (argc < 3) {
//         fprintf(stderr, "Usage: %s <filename> <generations>\n", argv[0]);
//         return EXIT_FAILURE;
//     }

//     int generations = atoi(argv[2]);
//     char* filename = argv[1];

//     int screen_ratio_width = 16;
//     int screen_ratio_height = 9;

//     int resolution_scaling_factor = 3;

//     int board_width = screen_ratio_width * resolution_scaling_factor;
//     int board_height = screen_ratio_height * resolution_scaling_factor;

//     bool* start_pattern = read_start_pattern(filename, board_width, board_height);
//     // randomize_pattern(start_pattern, board_width, board_height, 20); //comment this out to use the start_pattern.txt
//     if (start_pattern == NULL) {
//         return EXIT_FAILURE;
//     }

//     // run main loop
//     bool* current_pattern = start_pattern;
//     for (int i = 0; i < generations; i++) {
//         print_pattern(current_pattern, board_width, board_height);
//         usleep(100000); // .1 second
//         bool* next_pattern = generate_next_pattern(current_pattern, board_width, board_height);
//         free(current_pattern);
//         current_pattern = next_pattern;
//     }

//     return 0;
// }

/*
    Function to read a pattern from a file and center on a board
*/
int* read_start_pattern(char* filename, int max_width, int max_height) {
    FILE* file = fopen(filename, "r"); //open file in read mode
    if (file == NULL) {
        perror("Failed to open file"); //error message
        exit(EXIT_FAILURE);
    }

    // First pass to determine pattern dimensions
    int pattern_width = 0;
    int pattern_height = 0;
    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        pattern_height++;
        int line_length = strlen(line);
        if (line[line_length - 1] == '\n') { //if new line character
            line_length--; //remove
        }
        if (line_length > pattern_width) {
            pattern_width = line_length; //update maximum line length
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

/*
    Function to generate the next board state pased on the current pattern
*/
int* gol_gen_next(int* pattern, int width, int height) {
    int* next_pattern = (int*)malloc(width * height * sizeof(int));
    if (next_pattern == NULL) {
        perror("Failed to allocate memory for next pattern");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int cell_index = y * width + x;
            int live_neighbors = gol_count_live_neighbors(pattern, width, height, cell_index);

            if (pattern[cell_index]) {
                next_pattern[cell_index] = (live_neighbors == 2 || live_neighbors == 3);
            } else {
                next_pattern[cell_index] = (live_neighbors == 3);
            }
        }
    }

    return next_pattern;
}

/*
    Function to count the number of live neighbors for a given cell
*/
int gol_count_live_neighbors(int* pattern, int width, int height, int cell_index){
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

/*
    Functin to print the current pattern state to the console
*/
void print_pattern(int* pattern, int width, int height) {
    system("clear");
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%c", pattern[y * width + x] ? 'O' : '-');
        }
        printf("\n");
    }
    printf("\n");
}

/*
    Function to generate random board patterns based on percentage of alive cells
*/
int* gol_gen_random(int width, int height, int percent_alive) {
    /* Generates a random board on the heap */
    int* pattern = (int*)malloc(width * height * sizeof(int));
    if (pattern == NULL) {
        perror("Failed to allocate memory for random pattern");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    for (int i = 0; i < width * height; i++) {
        if (rand() % 100 < percent_alive) {
            pattern[i] = 1;
        } else {
            pattern[i] = 0;
        }
    }

    return pattern;
}

/*
    Function add some random cells as alive to a pattern
*/
void gol_add_life(int* pattern, int width, int height, int percent_alive) {
    // Airdrop some extra cells!!
    srand(time(NULL));
    for (int i = 0; i < width * height; i++) {
        if (pattern[i] == 0) {
            pattern[i] = (rand() % 100) < percent_alive;
        }
    }
}
