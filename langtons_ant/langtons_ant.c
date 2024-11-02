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


typedef struct {
    int x;
    int y;
    int direction; // 0:UP, 1:RIGHT, 2:DOWN, 3:LEFT
} Ant;


void ant_gen_next(int* grid, int width, int height, Ant* ants, int num_ants, const char* ruleset) {
    for (int i = 0; i < num_ants; i++) {
        int current_index = ants[i].y * width + ants[i].x;
        char current_rule = ruleset[grid[current_index]];
        
        // Update grid state
        grid[current_index] = (grid[current_index] + 1) % strlen(ruleset);
        
        // Update direction
        if (current_rule == 'L') {
            ants[i].direction = (ants[i].direction + 3) % 4;
        } else if (current_rule == 'R') {
            ants[i].direction = (ants[i].direction + 1) % 4;
        } else if (current_rule == 'U') {
            ants[i].direction = (ants[i].direction + 2) % 4;
        }
        
        // Move ant
        switch(ants[i].direction) {
            case 0: ants[i].y--; break; // Up
            case 1: ants[i].x++; break; // Right
            case 2: ants[i].y++; break; // Down
            case 3: ants[i].x--; break; // Left
        }
        
        // Wrap around edges
        ants[i].x = (ants[i].x + width) % width;
        ants[i].y = (ants[i].y + height) % height;
    }
}


void print_board(int* grid, int width, int height, Ant* ants, int num_ants) {
    int ant_indeces[num_ants]; // Use num_ants instead of sizeof(ants)
    for (int i = 0; i < num_ants; i++) {
        ant_indeces[i] = ants[i].y * width + ants[i].x;
    }
    
    for (int i = 0; i < width * height; i++) {
        int ant_here = 0;
        for (int j = 0; j < num_ants; j++) {
            if (i == ant_indeces[j]) { // Check against each ant index
                ant_here = 1;
                break;
            }
        }
        if (ant_here) {
            printf("^");
        } else {
            printf("%d", grid[i]);
        }
        if ((i + 1) % width == 0) {
            printf("\n");
        }
    }
}


int main(){
    int width = 150; // example width
    int height = 60; // example height
    int ant_x = width / 2;
    int ant_y = height / 2;
    int direction = 0; // default direction

    // index corresponds to rule number 
    // ex)[R, L]: rule 0 = turn right upon encountering zero, rule 1 = turn left upon encountering one
    // Acceptable rules include R:RIGHT, L:LEFT, C:CONTINUE, U:U-TURN
    const char ruleset[] = "RL";

    int grid[width * height];
    for (int i = 0; i < width * height; i++){ // init grid to all zeroes
        grid[i] = 0;
    }

    Ant my_aunt = {ant_x, ant_y, direction}; // initialize ant(s) and put in ant array
    Ant ants[] = {my_aunt};
    int num_ants = sizeof(ants) / sizeof(ants[0]);

    while(1) {
        system("clear");
        print_board(grid, width, height, ants, num_ants);
        ant_gen_next(grid, width, height, ants, num_ants, ruleset);
        usleep(10000);
    }
    return 0;
}
