/* simwall.c
Main driver code for the simwall project on Linux
Heavily abstracted away into not-so-pretty libraries
*/
#ifndef SIMWALL_C
#define SIMWALL_C

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "x11_lib.h"
#include "c_version/game_of_life.h"

int main(int argc, char **argv) {
    // parse arguments
    unsigned char flags = parse_args(argc, argv);

    // Initialize the window
    window_setup(flags);

    // Set up event handler
    // XEvent event; old event handling code

    // LIFE TIME!!!    
    int pix_height = screen_height();
    int pix_width = screen_width();

    int board_height = pix_height / CELL_SIZE;
    int board_width = pix_width / CELL_SIZE;

    // Set up the game of life board with random start
    bool* current_pattern = malloc(board_width * board_height * sizeof(bool));
    randomize_pattern(current_pattern, board_width, board_height);

    /* Could optimize this to only update when the user is looking at it
    I'm just not sure how to detect that yet. Looks like in-built event
    handling in X11 but I've been trying for an hour to make it work and
    I cannot. */
    
    // Fill the background with black just in case we don't fill up the whole screen
    color(0, 0, 0);
    fill_background();

    while (1) {
        // loop through our board and draw it
        for (int i = 0; i < board_width * board_height; i++) {
            // check current cell
            if (current_pattern[i] == 1) {
                // if alive, draw white
                color(255, 255, 255);
                fill_cell(i % board_width, i / board_width);
            }
            else 
            {
                // if dead, draw black
                color(0, 0, 0);
                fill_cell(i % board_width, i / board_width);
            }
        }

        // fill one more row and col with black to make sure we fill the whole screen
        for (int i = 0; i < board_width; i++) {
            color(0, 0, 0);
            fill_cell(i, board_height);
        }

        for (int i = 0; i < board_height; i++) {
            color(0, 0, 0);
            fill_cell(board_width, i);
        }

        usleep(100000); // .1 second

        // Now generate the next pattern
        // XNextEvent(display, &event); old event handling code
        bool* next_pattern = generate_next_pattern(current_pattern, board_width, board_height);
        // freeing of old pattern handled in generate_next_pattern
        current_pattern = next_pattern;
    }

    free(current_pattern);
    cleanup();
    return 0;
}

#endif // SIMWALL_C