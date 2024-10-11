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
#define KEYBINDS    1 << 2

int CELL_SIZE = 25;

bool add_mode = false;

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
    fprintf(stderr, "  -c: Draw circles instead of a squares\n");
    fprintf(stderr, "  -s 25: Set the size of the cells\n");
    fprintf(stderr, "  -nk: Disable keybinds\n");
    fprintf(stderr, "Example: simwall -bg FF00FF -fg 00FF00 -fps 10.0\n");
}

Args* parse_args(int argc, char **argv) {
    /* Parses the args for the program.
    Takes argc, argv from cmdline.
    !! FREE THE RETURNED ARGS !! */
    // define defaults
    Args* args = malloc(sizeof(Args));
    memset(args, 0, sizeof(Args));
    args->flags |= KEYBINDS; // set keybinds to default to on
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
        // disable keybinds
        else if (strcmp(argv[i], "-nk") == 0) {
            args->flags &= ~KEYBINDS;
        }
        else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            usage();
            exit(1);
        }
    }

    return args;
}

void handle_keybinds() {
    /* Handles the keybinds for the program
    Meant to be run in the main loop
    Takes the filling function being used */

    // Quit if Ctrl-Alt-Q is pressed
    if (check_for_keybind("Q")) {
        x11_cleanup();
        exit(0);
    }

    // Pause if Ctrl-Alt-P is pressed
    if (check_for_keybind("P")) {
        wait_for_keybind("P");
    }

    // Enter add mode if Ctrl-Alt-A is pressed
    if (check_for_keybind("A")) {
        add_mode = !add_mode;
    }
}

/* Could optimize this to only update when the user is looking at it,
but I'm just not sure how to detect that yet. Looks like in-built event
handling in X11 but I've been trying for an hour to make it work and
I cannot. Maybe the answer is to look at the root window and check 
its events? Cause the root window is overlayed on the sim */
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

    // Initialize the window
    Display* display = window_setup(args->bg_color);
    
    // Set up add, pause, and quit keybinds
    if (args->flags & KEYBINDS) {
        setup_keybind("A");
        setup_keybind("P");
        setup_keybind("Q");
    }

    // LIFE TIME!!!    
    int board_height = screen_height() / CELL_SIZE;
    int board_width = screen_width() / CELL_SIZE;

    // Set up the game of life board with random start
    bool* current_pattern = malloc(board_width * board_height * sizeof(bool));
    randomize_pattern(current_pattern, board_width, board_height, 20);

    // track how many dead there are
    float dead = 0;
    const float total = board_width * board_height;

    // define the colors as ints for X11
    int bg_color_int = rgb_to_int(args->bg_color);
    int fg_color_int = rgb_to_int(args->fg_color);

    // set the color to the background color
    int cur_color = bg_color_int;
    color(args->bg_color);

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
                color(args->fg_color);
            }
            else if (current_pattern[i] == 0) {
                // if dead, increment dead count
                dead++;
                if (cur_color != bg_color_int) {
                    // if we're not already on the bg color, switch to it
                    cur_color = bg_color_int;
                    color(args->bg_color);
                }
            }

            // fill the cell with whatever color we land on
            (*fill_func)(i % board_width, i / board_width, CELL_SIZE);
        }

        color(args->bg_color);
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

        // keybind processing
        if (args->flags & KEYBINDS) {
            handle_keybinds();
        }

        // Add cells if in add mode
        if (add_mode) {
            // set the color to the cell color
            color(args->fg_color);

            // loop until we're out of add mode (effectively pause)
                // this is to prevent placed cells from instantly dying
            while (add_mode) {
                // if the left mouse button is being pressed
                if (is_lmb_pressed()) {
                    // get the mouse position
                    POS mouse_pos = get_mouse_pos();
                    int x = mouse_pos.x / CELL_SIZE;
                    int y = mouse_pos.y / CELL_SIZE;

                    // fill the cell
                    current_pattern[y * board_width + x] = 1;
                    fill_func(x, y, CELL_SIZE);
                }

                // handle keybinds to allow escape from here
                handle_keybinds();
                // sleep a while
                usleep(10000);
            }
        }


        // sleep for the remainder of the frame time, which is usually 100% of it
        time_t end_time = time(NULL);
        int sleep_time = 1000000 / args->framerate - (end_time - start_time);
        if (sleep_time > 0) {
            usleep(sleep_time);
        }
    }

    // cleanup
    free(args);
    free(current_pattern);
    x11_cleanup();
    return 0;
}

#endif // SIMWALL_C