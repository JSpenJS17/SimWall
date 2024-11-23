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
#include "game_of_life/game_of_life.h"
#include "brians_brain/brians_brain.h"
#include "seeds/seeds.h"
#include "langtons_ant/langtons_ant.h"

#define DAEMONIZE   1
#define CIRCLE      (1 << 1)
#define KEYBINDS    (1 << 2)
#define BB          (1 << 3)
#define CLEAR       (1 << 4)
#define NO_RESTOCK  (1 << 5)
#define SEEDS       (1 << 6)
#define ANT         (1 << 7)

/* General purpose cmd-line args */
typedef struct Args {
    ARGB alive_color, dead_color, dying_color;
    uchar flags;
    Ant* ants;
    size_t num_ants;
    float framerate;
} Args;

/* Struct to store board information */
typedef struct Board {
    int width, height;
    int* pattern;
} Board;

// Globals
size_t CELL_SIZE = 25;
bool add_mode = false;
void (*fill_func)(int, int, size_t); // x, y, size
Args* args;
int cur_color;
ARGB* color_list;

// Langton's Ant specific globals
size_t num_colors;

void usage() {
    fprintf(stderr, "Usage: simwall [options]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help: Show this help message\n");
    fprintf(stderr, "  -D, -d, --daemonize: Daemonize the process\n");
    fprintf(stderr, "  -dead 000000FF: Set the dead cell color (RGBA)\n");
    fprintf(stderr, "  -alive FFFFFFFF: Set the alive cell color (RGBA)\n");
    fprintf(stderr, "  -dying 808080FF: Set the dying cell color (RGBA)\n");
    fprintf(stderr, "  -fps 10.0: Set the framerate\n");
    fprintf(stderr, "  -bb: Run Brian's Brain (BB) instead of Game of Life\n");
    fprintf(stderr, "  -seeds: Run Seeds instead of Game of Life\n");
    fprintf(stderr, "  -ant: Run Langton's Ant instead of Game of Life\n");
    fprintf(stderr, "    -color_list 000000FF 808080FF FFFFFFFF ... : Set the color list for states in Langton's Ant (RGBA)\n");
    fprintf(stderr, "    -ants ants.txt: Give input ant locations and directions in a file.\n");
    fprintf(stderr, "       Format: x y direction ruleset color\\n\n");
    fprintf(stderr, "         For direction, 0:UP, 1:RIGHT, 2:DOWN, 3:LEFT\n"); // might change to strings that are parsed to the enum later
    fprintf(stderr, "         For ruleset, R:RIGHT, L:LEFT, C:CONTINUE, U:U-TURN\n");
    fprintf(stderr, "      example: 5 5 0 RL 000000FF\n");
    fprintf(stderr, "  -c: Draw circles instead of a squares\n");
    fprintf(stderr, "  -s 25: Set the cell size in pixels\n");
    fprintf(stderr, "  -nk: Disable keybinds\n");
    fprintf(stderr, "  -nr: No restocking if board is too empty\n");
    fprintf(stderr, "  -clear: Start with a clear board. Includes -nr\n");
    fprintf(stderr, "Example: simwall -dead FF00FFFF -alive FFFF00FF -fps 7.5\n");
    exit(1);
}

void cleanup() {
    /* Cleans up the program */
    free(color_list);
    free(args->ants);
    free(args);
    x11_cleanup();
    exit(0);
}

int count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return -1; // Indicate error
    }

    int line_count = 0;
    char ch;

    // Read file character by character
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            line_count++;
        }
    }

    fclose(file);
    return line_count;
}

Args* parse_args(int argc, char **argv) {
    /* Parses the args for the program.
    Takes argc, argv from cmdline.
    !! FREE THE RETURNED ARGS !! */
    // define defaults
    Args* args = malloc(sizeof(Args));
    memset(args, 0, sizeof(Args));

    // define all the simulation flags, used for mutual exclusion later
    int all_sims = BB | SEEDS | ANT;

    // set defaults
    args->flags |= KEYBINDS; // set keybinds to default to on
    args->framerate = 10.0;
    
    args->alive_color.a = 255;
    args->alive_color.r = 255;
    args->alive_color.g = 255;
    args->alive_color.b = 255;

    args->dead_color.a = 255;
    args->dead_color.r = 0;
    args->dead_color.g = 0;
    args->dead_color.b = 0;

    args->dying_color.a = 255;
    args->dying_color.r = 128;
    args->dying_color.g = 128;
    args->dying_color.b = 128;

    // parse the args
    for (int i = 1; i < argc; i++) {
        // help
        if (strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0) {
            usage();
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
        // dead color
        else if (strcmp(argv[i], "-dead") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -dead\n");
                usage();
            }
            // convert the hex to ARGB
            char* dead_color_str = argv[i+1];
            if (strlen(dead_color_str) != 8) {
                fprintf(stderr, "Invalid color: %s\n", dead_color_str);
                usage();
            }
            // Use sscanf to convert hex to ARGB
            sscanf(dead_color_str, "%2hx%2hx%2hx%2hx", &args->dead_color.r, &args->dead_color.g, &args->dead_color.b, &args->dead_color.a);
            i++; // increment i to simulate parsing the hex string
        }
        // dying color
        else if (strcmp(argv[i], "-dying") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -dying\n");
                usage();
            }
            // convert the hex to ARGB
            char* dying_color_str = argv[i+1];
            if (strlen(dying_color_str) != 8) {
                fprintf(stderr, "Invalid color: %s\n", dying_color_str);
                usage();
            }
            // Use sscanf to convert hex to ARGB
            sscanf(dying_color_str, "%2hx%2hx%2hx%2hx", &args->dying_color.r, &args->dying_color.g, &args->dying_color.b, &args->dying_color.a);
            i++; // increment i to simulate parsing the hex string
        }
        // alive color
        else if (strcmp(argv[i], "-alive") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -alive\n");
                usage();
            }
            // convert the hex to ARGB
            char* alive_color_str = argv[i+1];
            if (strlen(alive_color_str) != 8) {
                fprintf(stderr, "Invalid color: %s\n", alive_color_str);
                usage();
            }
            // Use sscanf to convert hex to ARGB
            sscanf(alive_color_str, "%2hx%2hx%2hx%2hx", &args->alive_color.r, &args->alive_color.g, &args->alive_color.b, &args->alive_color.a);
            i++; // increment i to simulate parsing the hex string
        } 
        // framerate
        else if (strcmp(argv[i], "-fps") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -fps\n");
                usage();
            }
            args->framerate = atof(argv[i+1]);
            i += 1; 
        }
        // cell size
        else if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -s\n");
                usage();
            }
            CELL_SIZE = atoi(argv[i+1]);
            i += 1;
        }
        // color list for langton's ant
        else if (strcmp(argv[i], "-color_list") == 0) {
            // Find the number of colors
            num_colors = 0;
            for (int j = i + 1; j < argc; j++) {
                if (argv[j][0] == '-') {
                    break;
                }
                num_colors++;
            }
            // Allocate space for the colors
            color_list = (ARGB*)malloc(num_colors * sizeof(ARGB));

            // Parse the colors
            for (int j = 0; j < num_colors; j++) {
                char* color_str = argv[i+j+1];
                if (strlen(color_str) != 8) {
                    fprintf(stderr, "Invalid color: %s\n", color_str);
                    usage();
                }
                sscanf(color_str, "%2hx%2hx%2hx%2hx", &color_list[j].r, &color_list[j].g, &color_list[j].b, &color_list[j].a);
            }
            i += num_colors;

            // Error out if no colors given
            if (num_colors == 0) {
                fprintf(stderr, "No colors given for Langton's Ant\n");
                usage();
            }
        }
        // ants file
        else if (strcmp(argv[i], "-ants") == 0) {
            // free the default ant
            free(args->ants);
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -ants\n");
                usage();
            }
            // open the file
            FILE* ants_file = fopen(argv[i+1], "r");
            if (!ants_file) {
                fprintf(stderr, "Could not open file: %s\n", argv[i+1]);
                usage();
            }
            // read the number of ants
            args->num_ants = count_lines(argv[i+1]);
            // allocate space for the ants
            args->ants = (Ant*)malloc(args->num_ants * sizeof(Ant));
            // read the ants
            for (int j = 0; j < args->num_ants; j++) {
                Ant ant;
                char line[512];
                // read in a line
                fgets(line, 512, ants_file);
                // parse the line
                int num = sscanf(line, "%d %d %d %s %2hx%2hx%2hx%2hx\n",
                            &ant.x, &ant.y, (int*)&ant.direction,
                            ant.ruleset,
                            &ant.color.r, &ant.color.g, &ant.color.b, &ant.color.a);
                // check for errors (lightly, not a full check)
                if (num != 8) {
                    fprintf(stderr, "Invalid ant line: %s\n", line);
                    usage();
                }
                // add the ant to the list
                args->ants[j] = ant;
            }
            // remember to close the file
            fclose(ants_file);
            i += 1;
        }
        // disable keybinds
        else if (strcmp(argv[i], "-nk") == 0) {
            args->flags &= ~KEYBINDS;
        }
        // brians brain
        else if (strcmp(argv[i], "-bb") == 0) {
            args->flags = (args->flags & ~all_sims) | BB;
        }
        // seeds
        else if (strcmp(argv[i], "-seeds") == 0) {
            args->flags = (args->flags & ~all_sims) | SEEDS;
        }
        // langton's ant
        else if (strcmp(argv[i], "-ant") == 0) {
            args->flags = (args->flags & ~all_sims) | ANT;
            args->flags |= NO_RESTOCK;
        }
        // clear start
        else if (strcmp(argv[i], "-clear") == 0) {
            args->flags |= NO_RESTOCK;
            args->flags |= CLEAR;
        }
        // no restocking
        else if (strcmp(argv[i], "-nr") == 0) {
            args->flags |= NO_RESTOCK;
        }
        else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            usage();
        }
    }

    return args;
}

void handle_keybinds(Board* cur_board) {
    /* Handles the keybinds for the program
    Meant to be run in the main loop
    Takes the filling function being used */

    // Quit if Ctrl-Alt-Q is pressed
    if (check_for_keybind("Q")) {
        // cleanup
        cleanup();
        exit(0);
    }

    // Pause if Ctrl-Alt-P is pressed
    if (check_for_keybind("P")) {
        wait_for_keybind("P");
    }

    // Enter add mode if Ctrl-Alt-A is pressed
    if (check_for_keybind("A") && !add_mode) {
        // set add_mode to true to prevent extra recursion into here
        add_mode = true;

        // set the color to the cell color
        color(args->alive_color);
        cur_color = ALIVE;

        // loop until we're out of add mode (effectively pause)
            // this is to prevent placed cells from instantly dying
        while (!check_for_keybind("A")) {
            // if the left mouse button is being pressed
            if (is_lmb_pressed()) {
                // get the mouse position
                POS mouse_pos = get_mouse_pos();
                int x = mouse_pos.x / CELL_SIZE;
                int y = mouse_pos.y / CELL_SIZE;

                // fill the cell
                cur_board->pattern[y * cur_board->width + x] = ALIVE;
                fill_func(x, y, CELL_SIZE);
            }

            // also have keybind handling here
            handle_keybinds(cur_board);
            // sleep a while
            usleep(10000);
        }

        // set it to false now that we're out
        add_mode = false;
    }

    // Clear the board if Ctrl-Alt-D is pressed
    if (check_for_keybind("D")) {
        // Set the board to deads
        memset(cur_board->pattern, DEAD, cur_board->width * cur_board->height * sizeof(int));

        // Set the color
        color(args->dead_color);
        cur_color = DEAD;

        // Fill the board right here and now for instant updates!
        for (int i = 0; i < cur_board->width * cur_board->height; i++) {
            (*fill_func)(i % cur_board->width, i / cur_board->width, CELL_SIZE);
        }
    }
}

/* Could optimize this to only update when the user is looking at it,
but I'm just not sure how to detect that yet. Looks like in-built event
handling in X11 but I've been trying for an hour to make it work and
I cannot. Maybe the answer is to look at the root window and check 
its events? Cause the root window is overlayed on the sim */
int main(int argc, char **argv) {
    // parse arguments
    args = parse_args(argc, argv);

    // daemonize if they asked for it
    if (args->flags & DAEMONIZE) {
        int pid = fork();
        if (pid != 0) {
            return 0;
        }
    }

    // Initialize the window
    window_setup(args->dead_color);
    
    // Set up add, pause, delete (clear), and quit keybinds
    if (args->flags & KEYBINDS) {
        setup_keybind("A");
        setup_keybind("P");
        setup_keybind("Q");
        setup_keybind("D");
    }

    // set the fill function based on the flags
    fill_func = args->flags & CIRCLE ? fill_circle : fill_cell; // (x, y, size)

    int* (*gen_next)(int*, int, int);
    int* (*gen_random)(int, int, int);
    void (*add_random)(int*, int, int, int);
    // set the generation functions based on the flags
    if (args->flags & BB) {
        gen_next = bb_gen_next;
        gen_random = bb_gen_random;
        add_random = bb_add_life;
    } else if (args->flags & SEEDS) {
        gen_next = seeds_gen_next;
        gen_random = seeds_gen_random;
        add_random = seeds_add_life;
    } else if (args->flags & ANT) { 
        gen_next = ant_gen_next;
        gen_random = ant_gen_random;
        add_random = ant_add_life;
    } else {
        gen_next = gol_gen_next;
        gen_random = gol_gen_random;
        add_random = gol_add_life;
    }

    // set the restock threshold based on the flags
    float restock_thresh = args->flags & BB ? 1.0 : .95;

    // GAME TIME!!!    
    Board cur_board;
    cur_board.height = screen_height() / CELL_SIZE + 1;
    cur_board.width = screen_width() / CELL_SIZE + 1;
    
    // Set up the board with random start
    cur_board.pattern = (*gen_random)(cur_board.width, cur_board.height, 20);
    if (args->flags & CLEAR) {
        memset(cur_board.pattern, 0, cur_board.width * cur_board.height * sizeof(int));
    }

    // track how many dead there are
    float dead = 0;
    const float total = cur_board.width * cur_board.height;

    // set the color to the background color
    if (args->flags & ANT) {
        // do ant things
        if (!args->ants) {
            // Fill in default ant if none given
            args->num_ants = 1;
            args->ants = (Ant*)malloc(sizeof(Ant));
            args->ants[0].x = cur_board.width / 2;
            args->ants[0].y = cur_board.height / 2;
            args->ants[0].direction = UP;
            strcpy(args->ants[0].ruleset, "RL");
            args->ants[0].color = (ARGB){255, 255, 0, 0};
        }
        // Initialize the ants
        init_ants(args->ants, args->num_ants);

        // Find the longest ruleset
        size_t max_rules = 0;
        for (int i = 0; i < args->num_ants; i++) {
            size_t len = strlen(args->ants[i].ruleset);
            if (len > max_rules) {
                max_rules = len;
            }
        }

        // generate a color list if none given
        if (num_colors == 0) {
            // Set correct number of colors
            num_colors = max_rules;
            // Alloc them
            color_list = (ARGB*)malloc(max_rules * sizeof(ARGB));
            // Generate them by stepping through the RGB spectrum in equal steps
                // this will create a number of shades of gray
            int step_size = 256 / max_rules;
            // Set the colors
            for (int i = 0; i < max_rules; i++) {
                color_list[i] = (ARGB){255, i * step_size, i * step_size, i * step_size};
            }
        }

        // Make sure the number of colors given is >= the number of states
        if (num_colors < max_rules) {
            fprintf(stderr, "Not enough colors given for Langton's Ant\n");
            exit(1);
        }
    } else {
        num_colors = 3;
        color_list = (ARGB*)malloc(3 * sizeof(ARGB));
        color_list[0] = args->dead_color;
        color_list[1] = args->alive_color;
        color_list[2] = args->dying_color;
    }
    
    color(color_list[cur_color]);
    cur_color = DEAD;    

    // define iter count
    int iter_count = 0;

    // Main loop
    while (1) {
        // get start time
        time_t start_time = time(NULL);

        /* DRAWING PORTION */
        // loop through our board and draw it
        for (int i = 0; i < cur_board.width * cur_board.height; i++) {
            // if the color has changed
            if (cur_board.pattern[i] != cur_color) {
                // update color accordingly
                    // % num_colors to allow for ANT's variable number of states
                cur_color = cur_board.pattern[i] % num_colors; 
                color(color_list[cur_color]);
            }

            // increment dead counter if on dead cell
                // will increment in langton's ant, but will do nothing
            if (cur_color == DEAD) {
                dead++;
            }

            // fill the cell with whatever color we land on
            fill_func(i % cur_board.width, i / cur_board.width, CELL_SIZE);
        }
        
        color(color_list[DEAD]);
        cur_color = DEAD;
        // fill one more row and col with bg to make sure we fill the whole screen
        for (int i = 0; i < cur_board.width; i++) {
            fill_func(i, cur_board.height, CELL_SIZE);
        }
        for (int i = 0; i < cur_board.height; i++) {
            fill_func(cur_board.width, i, CELL_SIZE);
        }

        // Handle drawing ants over the now completed board
        if (args->flags & ANT) {
            // Loop through the ants and draw them
            cur_color = -1; // dummy value to let us know we need to reset the color
            for (int ant_index = 0; ant_index < args->num_ants; ant_index++) {
                Ant ant = args->ants[ant_index];
                color(args->ants[ant_index].color);
                fill_func(ant.x, ant.y, CELL_SIZE);
            }
        }

        /* GENERATION PORTION */
        // Now generate the next pattern
        int* next_pattern = (*gen_next)(cur_board.pattern, cur_board.width, cur_board.height);
        free(cur_board.pattern);
        cur_board.pattern = next_pattern;

        // check if we need to add more cells
        if (args->flags & SEEDS) {
            // increment iter count
            iter_count++;
            // if iter count too high
            if (iter_count >= 100 && !(args->flags & NO_RESTOCK)) {
                int* next_pattern = gen_random(cur_board.width, cur_board.height, 20);
                free(cur_board.pattern);
                cur_board.pattern = next_pattern;
                iter_count = 0;
            }
        } else {
            // if XX% of the board is dead, add more cells
            if (dead/total >= restock_thresh && !(args->flags & NO_RESTOCK)) {
                add_random(cur_board.pattern, cur_board.width, cur_board.height, 20);
                iter_count = 0;
            }
        }
        // reset dead count
        dead = 0;


        /* EXTRA FEATURES */
        // keybind processing
        if (args->flags & KEYBINDS) {
            handle_keybinds(&cur_board);
        }

        // sleep for the remainder of the frame time, which is usually 100% of it
        time_t end_time = time(NULL);
        int sleep_time = 1000000 / args->framerate - (end_time - start_time);
        if (sleep_time > 0) {
            usleep(sleep_time);
        }
    }

    // cleanup, not that this is reachable
    cleanup();
    x11_cleanup();
    return 0;
}

#endif // SIMWALL_C