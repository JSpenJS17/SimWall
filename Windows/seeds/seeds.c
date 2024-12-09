/*
Name: seeds.c

Authors:
    - Holden Vail

Creation Date: 10-07-2024
Description: This program reads a Seeds board state and generates some number of future board states according
            to the rules of Seeds
*/

#include <stdio.h> //standard I/O library
#include <stdlib.h> //standard library for general utility functions
#include <stdbool.h> //Boolean type
#include <windows.h> //Windows API header for windows-specific functions
#include <string.h> //string manipulation
#include <time.h> //time functions
#include <math.h> //math fucntions
#include "seeds.h" //include seeds header file

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

/*
    Function to read the starting pattern from a given file
*/
int* seeds_read_start_pattern(char* filename, int max_width, int max_height) {
    FILE* file = fopen(filename, "r"); //open file in read mode
    if (file == NULL) { //if NULL
        perror("Failed to open file"); //print error message
        exit(EXIT_FAILURE); //exit program with failure status
    }

    // First pass to determine pattern dimensions
    int pattern_width = 0; //initialize pattern width to 0
    int pattern_height = 0; //initialize pattern height to 0
    char line[1024]; //temporary storage for file line

    while (fgets(line, sizeof(line), file)) { //read each line from file
        pattern_height++; //increment pattern height for each line
        int line_length = strlen(line); //get length of line
        if (line[line_length - 1] == '\n') { //if line is newline character
            line_length--; //remove newline character
        }
        if (line_length > pattern_width) { //if line length is greater than pattern width
            pattern_width = line_length; //update pattern width
        }
    }

    // Check if the pattern is larger than the max dimensions
    if (pattern_width > max_width || pattern_height > max_height) {
        fprintf(stderr, "Pattern is larger than the max dimensions (%d x %d)\n", max_width, max_height);
        fclose(file); //close file
        exit(EXIT_FAILURE); //exit program with failure status
    }

    // Allocate memory for the full board
    int* board = (int*)calloc(max_width * max_height, sizeof(int)); //allocate memory for baord
    if (board == NULL) { //if NULL
        perror("Failed to allocate memory for board"); //print error message
        fclose(file); //close file
        exit(EXIT_FAILURE); //exit program with failure status
    }

    // Calculate starting position to center the pattern
    int start_x = (max_width - pattern_width) / 2; //calculate start x position
    int start_y = (max_height - pattern_height) / 2; //calculate start y position

    // Second pass to read the pattern and place it on the board
    fseek(file, 0, SEEK_SET); //reset file pointer
    int y = start_y; //initial y position
    while (fgets(line, sizeof(line), file)) { //read each line
        int line_length = strlen(line); //get length of line
        if (line[line_length - 1] == '\n') { //if newline character
            line_length--; //remove
        }
        for (int x = 0; x < line_length; x++) {
            board[(y * max_width) + (start_x + x)] = (line[x] == '1'); //set cell to 1 if alive, 0 otherwise
        }
        y++;
    }

    fclose(file); //close file
    return board; //return baord
}

/*
    Function to generate the next pattern based on current pattern
*/
int* seeds_gen_next(int* pattern, int width, int height) {
    int* next_pattern = (int*)malloc(width * height * sizeof(int)); //allocate memory for new pattern
    if (next_pattern == NULL) { //if memory allocation fails
        perror("Failed to allocate memory for next pattern"); //print error messsage
        exit(EXIT_FAILURE); //exit program with failure status
    }

    for (int y = 0; y < height; y++) { //loop over each row
        for (int x = 0; x < width; x++) { //loop over each column
            int cell_index = y * width + x; //calculate index for current cell
            int live_neighbors = seeds_count_live_neighbors(pattern, width, height, cell_index);
            //count number of live neighbors

            // Seeds rule: B2/S
            next_pattern[cell_index] = (!pattern[cell_index] && live_neighbors == 2); //apply Seeds rule
        }
    }

    return next_pattern; //return new next pattern
}

/*
    Function to count the number of live neighbors for a given cell
*/
int seeds_count_live_neighbors(int* pattern, int width, int height, int cell_index){
    int live_neighbors_count = 0; //initialize number of live neighbors to 0
    int cell_x = cell_index % width; //get x coordinate of cell
    int cell_y = cell_index / width; //get y coordinate of cell

    for (int delta_y = -1; delta_y <= 1; delta_y++) { //loop over Y offsets
        for (int delta_x = -1; delta_x <= 1; delta_x++) { //loop over X offsets
            if (delta_x == 0 && delta_y == 0) continue; // Skip the cell itself

            int neighbor_x = cell_x + delta_x; //calculate neighbor x coordinate
            int neighbor_y = cell_y + delta_y; //calculate neighbor y coordinate

            if (neighbor_x >= 0 && neighbor_x < width && neighbor_y >= 0 && neighbor_y < height) { //check if within grid boundaries
                int neighbor_index = neighbor_y * width + neighbor_x; //calculate neighbor's index
                if (pattern[neighbor_index] == 1) { //if neighbor index 1 = alive
                    live_neighbors_count++; //increment number of live neighbors
                }
            }
        }
    }
    return live_neighbors_count; //return number of live neighbors
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

/*
    Function to generate a randomly generated pattern
*/
int* seeds_gen_random(int width, int height, int percent_alive) {
    srand(time(NULL)); //random number generator
    int* pattern = (int*)malloc(width * height * sizeof(int)); //allocate memory for pattern
    memset(pattern, 0, width * height * sizeof(int));
    
    // Create a block of 6x6 cells in the middle
    int start_x = width / 2 - 3; //calculate start x position
    int start_y = height / 2 - 3; //calculate start y position
    
    for (int y = start_y; y < start_y + 6; y++) { //iterate over rows
        for (int x = start_x; x < start_x + 6; x++) { //iterate over columns
            pattern[y*width + x] = (rand() % 100) < percent_alive; //set cell to alive or dead based on percent
        }
    }
    return pattern; //return generated pattern
}

/*
    Function to add some new alive cells
*/
void seeds_add_life(int* pattern, int width, int height, int percent_alive) {
    // Airdrop some extra cells!!
    srand(time(NULL)); //random number generator
    for (int i = 0; i < width * height; i++) { //iterate over each cell
        if (pattern[i] == 0) { //if current cell index = 0 then dead
            pattern[i] = (rand() % 100) < percent_alive; //randomly set to alive 
        }
    }
}
