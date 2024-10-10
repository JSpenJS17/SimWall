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

#define DAEMONIZE 1

/* General purpose cmd-line args 
Can add more later if needed */
struct Args {
    RGB bg_color;
    RGB fg_color;
    uchar flags;
};
typedef struct Args Args;

void usage() {
    fprintf(stderr, "Usage: simwall [-h] [-bg r g b] [-fg r g b] [-d] \n");
}

Args* parse_args(int argc, char **argv) {
    /* Parses the args for the program.
    Takes argc, argv from cmdline.
    !! FREE THE RETURNED ARGS !! */
    // define defaults
    Args* args = malloc(sizeof(Args));
    memset(args, 0, sizeof(Args));
    args->fg_color.r = 255;
    args->fg_color.g = 255;
    args->fg_color.b = 255;

    // parse the args
    for (int i = 1; i < argc; i++) {
        // help
        if (strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0) {
            usage();
            exit(1);
        }
        // daemonize
        else if (strcmp(argv[i], "-D") == 0 || 
                 strcmp(argv[i], "--daemonize") == 0|| 
                 strcmp(argv[i], "-d") == 0) {
            args->flags |= DAEMONIZE;
        }
        // background color
        else if (strcmp(argv[i], "-bg") == 0) {
            if (i + 3 >= argc) {
                fprintf(stderr, "Not enough arguments for -bg\n");
                usage();
                exit(1);
            }
            args->bg_color.r = atoi(argv[i+1]);
            args->bg_color.g = atoi(argv[i+2]);
            args->bg_color.b = atoi(argv[i+3]);
            i += 3;
        }
        // foreground color
        else if (strcmp(argv[i], "-fg") == 0) {
            if (i + 3 >= argc) {
                fprintf(stderr, "Not enough arguments for -fg\n");
                usage();
                exit(1);
            }
            args->fg_color.r = atoi(argv[i+1]);
            args->fg_color.g = atoi(argv[i+2]);
            args->fg_color.b = atoi(argv[i+3]);
            i += 3;
        } 
        else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            usage();
            exit(1);
        }
    }

    return args;
}

int main(int argc, char **argv) {
    // parse arguments
    Args* args = parse_args(argc, argv);

    // daemonize if they asked for it
    if (args->flags & DAEMONIZE) {
        int pid = fork();
        if (pid != 0) {
            return 0;
        }
    }

    // Initialize the window -- might need to pass flags eventually
    window_setup();

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

    /* Could optimize this to only update when the user is looking at it,
    but I'm just not sure how to detect that yet. Looks like in-built event
    handling in X11 but I've been trying for an hour to make it work and
    I cannot. */
    
    // Fill the background with bg just in case we don't fill up the whole screen
    color_rgb(args->bg_color);
    fill_background();

    while (1) {
        // loop through our board and draw it
        for (int i = 0; i < board_width * board_height; i++) {
            // check current cell
            if (current_pattern[i] == 1) {
                // if alive, draw fg
                color_rgb(args->fg_color);
                fill_cell(i % board_width, i / board_width);
            }
            else 
            {
                // if dead, draw bg
                color_rgb(args->bg_color);
                fill_cell(i % board_width, i / board_width);
            }
        }

        // fill one more row and col with bg to make sure we fill the whole screen
        for (int i = 0; i < board_width; i++) {
            color_rgb(args->bg_color);
            fill_cell(i, board_height);
        }

        for (int i = 0; i < board_height; i++) {
            color_rgb(args->bg_color);
            fill_cell(board_width, i);
        }

        usleep(100000); // .1 second

        // Now generate the next pattern
        // XNextEvent(display, &event); old event handling code
        bool* next_pattern = generate_next_pattern(current_pattern, board_width, board_height);
        // freeing of old pattern handled in generate_next_pattern
        current_pattern = next_pattern;
    }

    // cleanup
    free(args);
    free(current_pattern);
    x11_cleanup();
    return 0;
}

#endif // SIMWALL_C