/*
Author: Holden Vail
Class: EECS 581
Assignment: Project 3
Creation Date: 10/29/2024

Langton's ant
*/

#include <stdio.h> //standard I/O library
#include <windows.h> //Windows API header for window specific functions
#include <string.h> //string manipulation library
#include <stdlib.h> //standard library for functions
#include "langtons_ant.h" //include Langton's Ant header file

// Globals to let this be imported as the others are
static int num_ants; //number of ants
static char* ruleset; //ruleset for Langton's Ant to determine behavior
static Ant* ants; //array of ants in the simulaton

/*
    Function to generate the next state of the grid based on current position of ants
*/
int* ant_gen_next(int* grid, int width, int height) {
    for (int i = 0; i < num_ants; i++) { //loop over each ant
        int current_index = ants[i].y * width + ants[i].x; //calculate current index of ant's position
        char current_rule = ruleset[grid[current_index]]; //get rules for the current cell
        
        // Update grid state
        grid[current_index] = (grid[current_index] + 1) % strlen(ruleset);
        
        // Update direction
        if (current_rule == 'L') {
            ants[i].direction = (ants[i].direction + 3) % NUM_DIRS; //turn left
        } else if (current_rule == 'R') {
            ants[i].direction = (ants[i].direction + 1) % NUM_DIRS; //turn right
        } else if (current_rule == 'U') {
            ants[i].direction = (ants[i].direction + 2) % NUM_DIRS; //u-turn
        }
        
        // Move ant
        switch(ants[i].direction) {
            case UP: ants[i].y--; break; //move ant up
            case RIGHT: ants[i].x++; break; //move ant right
            case DOWN: ants[i].y++; break; //move ant down
            case LEFT: ants[i].x--; break; //move ant left
            default:
                fprintf(stderr, "Invalid direction\n"); //print error message if invalid
                exit(1); //exit program
        }
        
        // Wrap around edges
        ants[i].x = (ants[i].x + width) % width; //wrap horizontally
        ants[i].y = (ants[i].y + height) % height; //wrap vertically
        
    }
    int* new_grid = (int*)malloc(width * height * sizeof(int)); //new grid to copy updated states
    memcpy(new_grid, grid, width * height * sizeof(int)); //copy current grid state to new_grid
    return new_grid; //return the new_grid
}

/*
    Function to initialize ants with given set of ants, number, and ruleset
*/
void init_ants(Ant* inp_ants, int inp_num_ants, char* inp_ruleset) {
    ants = inp_ants; //set array of ants
    ruleset = inp_ruleset; //set ruelset for simulation
    num_ants = inp_num_ants; //set number of ants
}

/*
    Function to add life to a pattern (theoretically, here for consistency with other simulation files)
*/
void ant_add_life(int* pattern, int width, int height, int percent_alive) {
    return; //empty function as Langton's Ant does not have this functionality
}

/*
    Function to generate a random grid (theoretically)
*/
int* ant_gen_random(int width, int height, int percent_alive) {
    //Langton's Ant does not have this exact functionality, instead generate blank board
    int* grid = (int*)malloc(width * height * sizeof(int)); //allocate memory for new grid
    memset(grid, 0, width * height * sizeof(int)); //initialize all cells to 0
    return grid; //return blank board
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
