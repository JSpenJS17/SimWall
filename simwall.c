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

#define DAEMONIZE   1
#define CIRCLE      1 << 1

int CELL_SIZE = 25;

/* General purpose cmd-line args 
Can add more later if needed */
struct Args {
    RGB bg_color;
    RGB fg_color;
    uchar flags;
    float framerate;
};
typedef struct Args Args;

void usage() {
    fprintf(stderr, "Usage: simwall [options]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help: Show this help message\n");
    fprintf(stderr, "  -D, -d, --daemonize: Daemonize the process\n");
    fprintf(stderr, "  -bg 000000: Set the background (dead cell) color\n");
    fprintf(stderr, "  -fg FFFFFF: Set the foreground (alive cell) color\n");
    fprintf(stderr, "  -fps 10.0: Set the framerate (can be decimal)\n");
    fprintf(stderr, "  -c: Draw a circle instead of a square\n");
    fprintf(stderr, "  -s 25: Set the size of the cells\n");
    fprintf(stderr, "Example: simwall -bg FF00FF -fg 00FF00 -fps 10.0\n");
}

Args* parse_args(int argc, char **argv) {
    /* Parses the args for the program.
    Takes argc, argv from cmdline.
    !! FREE THE RETURNED ARGS !! */
    // define defaults
    Args* args = malloc(sizeof(Args));
    memset(args, 0, sizeof(Args));
    args->framerate = 10.0;
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
        // circle shape
        else if (strcmp(argv[i], "-c") == 0) {
            args->flags |= CIRCLE;
        }
        // background color
        else if (strcmp(argv[i], "-bg") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -bg\n");
                usage();
                exit(1);
            }
            // convert the hex to RGB
            char* bg_color_str = argv[i+1];
            if (strlen(bg_color_str) != 6) {
                fprintf(stderr, "Invalid color: %s\n", bg_color_str);
                usage();
                exit(1);
            }
            // Use sscanf to convert hex to RGB
            sscanf(bg_color_str, "%2hx%2hx%2hx", &args->bg_color.r, &args->bg_color.g, &args->bg_color.b);
            i++; // increment i to simulate parsing the hex string
        }
        // foreground color
        else if (strcmp(argv[i], "-fg") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -fg\n");
                usage();
                exit(1);
            }
            // convert the hex to RGB
            char* fg_color_str = argv[i+1];
            if (strlen(fg_color_str) != 6) {
                fprintf(stderr, "Invalid color: %s\n", fg_color_str);
                usage();
                exit(1);
            }
            // Use sscanf to convert hex to RGB
            sscanf(fg_color_str, "%2hx%2hx%2hx", &args->fg_color.r, &args->fg_color.g, &args->fg_color.b);
            i++; // increment i to simulate parsing the hex string
        } 
        // framerate
        else if (strcmp(argv[i], "-fps") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -fps\n");
                usage();
                exit(1);
            }
            args->framerate = atof(argv[i+1]);
            i += 1; 
        }
        // cell size
        else if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -s\n");
                usage();
                exit(1);
            }
            CELL_SIZE = atoi(argv[i+1]);
            i += 1;
        }
        else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            usage();
            exit(1);
        }
    }

    return args;
}

/* Could optimize this to only update when the user is looking at it,
but I'm just not sure how to detect that yet. Looks like in-built event
handling in X11 but I've been trying for an hour to make it work and
I cannot. */
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

    // LIFE TIME!!!    
    int disp_height = screen_height();
    int disp_width = screen_width();

    int board_height = disp_height / CELL_SIZE;
    int board_width = disp_width / CELL_SIZE;

    // Set up the game of life board with random start
    bool* current_pattern = malloc(board_width * board_height * sizeof(bool));
    randomize_pattern(current_pattern, board_width, board_height, 20);

    // track how many dead there are
    float dead = 0;
    const float total = board_width * board_height;

    // int representations to save some code in the loop (i know, i know)
    int bg_color_int = args->bg_color.r << 16 | args->bg_color.g << 8 | args->bg_color.b;
    int fg_color_int = args->fg_color.r << 16 | args->fg_color.g << 8 | args->fg_color.b;

    // set the color to the background color
    int cur_color = bg_color_int;
    color_rgb(args->bg_color);

    // track first loop
        // this is bad but if done up here it doesn't display for some reason
    bool first = true;

    // define function pointer to fill function so we can
    // change it based on the shape
    void (*fill_func)(int, int, int) = args->flags & CIRCLE ? fill_circle : fill_cell;

    // Main loop
    while (1) {
        // get start time
        time_t start_time = time(NULL);

        /* DRAWING PORTION */
        // loop through our board and draw it
        for (int i = 0; i < board_width * board_height; i++) {
            // check current cell's alive state
            if (current_pattern[i] == 1 && cur_color != fg_color_int) {
                // if alive and we're not already on the fg color, switch to it
                cur_color = fg_color_int;
                color_rgb(args->fg_color);
            }
            else if (current_pattern[i] == 0) {
                // if dead, increment dead count
                dead++;
                if (cur_color != bg_color_int) {
                    // if we're not already on the bg color, switch to it
                    cur_color = bg_color_int;
                    color_rgb(args->bg_color);
                }
            }

            // fill the cell with whatever color we land on
            (*fill_func)(i % board_width, i / board_width, CELL_SIZE);
        }

        color_rgb(args->bg_color);
        cur_color = bg_color_int;
        // fill one more row and col with bg to make sure we fill the whole screen
        for (int i = 0; i < board_width; i++) {
            (*fill_func)(i, board_height, CELL_SIZE);
        }
        for (int i = 0; i < board_height; i++) {
            (*fill_func)(board_width, i, CELL_SIZE);
        }


        /* GENERATION PORTION */
        // Now generate the next pattern
        bool* next_pattern = generate_next_pattern(current_pattern, board_width, board_height);
        free(current_pattern);
        current_pattern = next_pattern;

        if (dead/total >= .95) {
            // if 95% of the board is dead, add more cells
            add_random(current_pattern, board_width, board_height, 10);
        }
        // reset dead count
        dead = 0;

        // sleep for the remainder of the frame time, which is usually 100% of it
        time_t end_time = time(NULL);
        int sleep_time = 1000000 / args->framerate - (end_time - start_time);
        if (sleep_time > 0) {
            usleep(sleep_time);
        }

        // if end of first loop, fill background JUST in case
        if (first) {
            fill_background();
            first = false;
        }
    }

    // cleanup
    free(args);
    free(current_pattern);
    x11_cleanup();
    return 0;
}

#endif // SIMWALL_C