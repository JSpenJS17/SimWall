/*
Author: Holden Vail
Class: EECS 581
Assignment: Project 3
Creation Date: 10/29/2024

Langton's ant
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "langtons_ant.h"

// Globals to let this be imported as the others are
static int num_ants;
static Ant* ants;

int* ant_gen_next(int* grid, int width, int height) {
    for (int i = 0; i < num_ants; i++) {
        int current_index = ants[i].y * width + ants[i].x;
        char current_rule = ants[i].ruleset[grid[current_index]];
        
        // Update grid state
        grid[current_index] = (grid[current_index] + 1) % strlen(ants[i].ruleset);
        
        // Update direction
        if (current_rule == 'L') {
            ants[i].direction = (ants[i].direction + 3) % NUM_DIRS;
        } else if (current_rule == 'R') {
            ants[i].direction = (ants[i].direction + 1) % NUM_DIRS;
        } else if (current_rule == 'U') {
            ants[i].direction = (ants[i].direction + 2) % NUM_DIRS;
        }
        
        // Move ant
        switch(ants[i].direction) {
            case UP: ants[i].y--; break;
            case RIGHT: ants[i].x++; break;
            case DOWN: ants[i].y++; break;
            case LEFT: ants[i].x--; break;
        }
        
        // Wrap around edges
        ants[i].x = (ants[i].x + width) % width;
        ants[i].y = (ants[i].y + height) % height;
    }
    int* new_grid = (int*)malloc(width * height * sizeof(int));
    memcpy(new_grid, grid, width * height * sizeof(int));
    return new_grid;
}

void init_ants(Ant* inp_ants, int inp_num_ants) {
    ants = inp_ants;
    num_ants = inp_num_ants;
}

void ant_add_life(int* pattern, int width, int height, int percent_alive) {
    // Would airdrop extra cells, but that's not how Ant works.
    // Will do nothing, is here so that it's consistent with the other functions
    return;
}

int* ant_gen_random(int width, int height, int percent_alive) {
    // Would generate a random board, but that's not how Ant works.
    // Will generate a blank board
    int* grid = (int*)malloc(width * height * sizeof(int));
    memset(grid, 0, width * height * sizeof(int));
    return grid;
}

// void print_board(int* grid, int width, int height, Ant* ants, int num_ants) {
//     int ant_indeces[num_ants]; // Use num_ants instead of sizeof(ants)
//     for (int i = 0; i < num_ants; i++) {
//         ant_indeces[i] = ants[i].y * width + ants[i].x;
//     }
    
//     for (int i = 0; i < width * height; i++) {
//         int ant_here = 0;
//         for (int j = 0; j < num_ants; j++) {
//             if (i == ant_indeces[j]) { // Check against each ant index
//                 ant_here = 1;
//                 break;
//             }
//         }
//         if (ant_here) {
//             printf("^");
//         } else {
//             printf("%d", grid[i]);
//         }
//         if ((i + 1) % width == 0) {
//             printf("\n");
//         }
//     }
// }

// int main(){
//     int width = 150; // example width
//     int height = 60; // example height

//     // index corresponds to rule number 
//     // ex)[R, L]: rule 0 = turn right upon encountering zero, rule 1 = turn left upon encountering one
//     // Acceptable rules include R:RIGHT, L:LEFT, C:CONTINUE, U:U-TURN
//     // ruleset = "RL"; // example ruleset

//     int grid[width * height];
//     memset(grid, 0, width * height * sizeof(int));

//     Ant my_ant = {width/2, height/2, 0}; // initialize ant(s) and put in ant array
//     ants = (Ant*)malloc(num_ants * sizeof(Ant)); // allocate memory depending on # of ants
//     ants[0] = my_ant; // add ant to array, could be a loop

//     init_ants(ants, 1, "RL");

//     while(1) {
//         system("clear");
//         print_board(grid, width, height, ants, num_ants);
//         ant_gen_next(grid, width, height);
//         usleep(10000);
//     }
//     return 0;
// }
