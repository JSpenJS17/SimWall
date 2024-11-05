/* simwall.c
Main driver code for the simwall project on Linux
Heavily abstracted away into not-so-pretty libraries
*/
#ifndef SIMWALL_C
#define SIMWALL_C

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>
#include <time.h>

#include "windows_lib.h"
#include "game_of_life/game_of_life.h"
#include "brians_brain/brians_brain.h"

#define DAEMONIZE   1
#define CIRCLE      1 << 1
#define KEYBINDS    1 << 2
#define BB          1 << 3
#define CLEAR       1 << 4
#define NO_RESTOCK  1 << 5

int CELL_SIZE = 25;



/* General purpose cmd-line args 
Can add more later if needed */
struct Args {
    RGB alive_color, dead_color, dying_color;
    unsigned char flags;
    float framerate;
};
typedef struct Args Args;

void usage() {
    fprintf(stderr, "Usage: simwall [options]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help: Show this help message\n");
    fprintf(stderr, "  -D, -d, --daemonize: Daemonize the process\n");
    fprintf(stderr, "  -dead 000000: Set the dead cell color\n");
    fprintf(stderr, "  -alive FFFFFF: Set the alive cell color\n");
    fprintf(stderr, "  -dying 808080: Set the dying cell color (BB only)\n");
    fprintf(stderr, "  -fps 10.0: Set the framerate\n");
    fprintf(stderr, "  -bb: Run Brian's Brain (BB) instead of Game of Life\n");
    fprintf(stderr, "  -c: Draw circles instead of a squares\n");
    fprintf(stderr, "  -s 25: Set the cell size in pixels\n");
    fprintf(stderr, "  -nk: Disable keybinds\n");
    fprintf(stderr, "  -nr: No restocking if board is too empty\n");
    fprintf(stderr, "  -clear: Start with a clear board. Includes -nr\n");
    fprintf(stderr, "Example: simwall -bg FF00FF -fg 00FF00 -fps 10.0\n");
}

Args* parse_args(int argc, char **argv) {
    /* Parses the args for the program.
    Takes argc, argv from cmdline.
    !! FREE THE RETURNED ARGS !! */
    // define defaults
    Args* args = malloc(sizeof(Args));
    memset(args, 0, sizeof(Args));

    // set defaults
    args->flags |= KEYBINDS; // set keybinds to default to on
    args->framerate = 10.0;
    args->alive_color.r = 255;
    args->alive_color.g = 255;
    args->alive_color.b = 255;
    args->dying_color.r = 128;
    args->dying_color.g = 128;
    args->dying_color.b = 128;

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
        // dead color
        else if (strcmp(argv[i], "-dead") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -dead\n");
                usage();
                exit(1);
            }
            // convert the hex to RGB
            char* dead_color_str = argv[i+1];
            if (strlen(dead_color_str) != 6) {
                fprintf(stderr, "Invalid color: %s\n", dead_color_str);
                usage();
                exit(1);
            }
            // Use sscanf to convert hex to RGB
            sscanf(dead_color_str, "%2hx%2hx%2hx", &args->dead_color.r, &args->dead_color.g, &args->dead_color.b);
            i++; // increment i to simulate parsing the hex string
        }
        // dying color
        else if (strcmp(argv[i], "-dying") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -dying\n");
                usage();
                exit(1);
            }
            // convert the hex to RGB
            char* dying_color_str = argv[i+1];
            if (strlen(dying_color_str) != 6) {
                fprintf(stderr, "Invalid color: %s\n", dying_color_str);
                usage();
                exit(1);
            }
            // Use sscanf to convert hex to RGB
            sscanf(dying_color_str, "%2hx%2hx%2hx", &args->dying_color.r, &args->dying_color.g, &args->dying_color.b);
            i++; // increment i to simulate parsing the hex string
        }
        // alive color
        else if (strcmp(argv[i], "-alive") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -alive\n");
                usage();
                exit(1);
            }
            // convert the hex to RGB
            char* alive_color_str = argv[i+1];
            if (strlen(alive_color_str) != 6) {
                fprintf(stderr, "Invalid color: %s\n", alive_color_str);
                usage();
                exit(1);
            }
            // Use sscanf to convert hex to RGB
            sscanf(alive_color_str, "%2hx%2hx%2hx", &args->alive_color.r, &args->alive_color.g, &args->alive_color.b);
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
        // brians brain
        else if (strcmp(argv[i], "-bb") == 0) {
            args->flags |= BB;
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
        STARTUPINFO si = {sizeof(STARTUPINFO)};
        PROCESS_INFORMATION pi;

        if (!CreateProcess(NULL, "simwall.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            fprintf(stderr, "Failed to daemonize\n");
            exit(1);
        }
    }

    // Initialize the window
    HWND hwnd = window_setup(args->dead_color);

    // Register the hotkeys 
    if (!RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_ALT, 'Q')) {
        printf("Failed to register hotkey for Ctrl+Alt+Q\n");
    }
    if (!RegisterHotKey(hwnd, 2, MOD_CONTROL | MOD_ALT, 'P')) {
        printf("Failed to register hotkey for Ctrl+Alt+P\n");
    }
    if (!RegisterHotKey(hwnd, 3, MOD_CONTROL | MOD_ALT, 'A')) {
        printf("Failed to register hotkey for Ctrl+Alt+A\n");
    }

    // set the fill function based on the flags
    void (*fill_func)(int, int, int) = args->flags & CIRCLE ? fill_circle : fill_cell;

    // set the generation functions based on the flags
    int* (*gen_next)(int*, int, int) = args->flags & BB ? bb_gen_next : gol_gen_next;
    int* (*gen_random)(int, int, int) = args->flags & BB ? bb_gen_random : gol_gen_random;
    void (*add_random)(int*, int, int, int) = args->flags & BB ? bb_add_life : gol_add_life;

    // set the restock threshold based on the flags
    float restock_thresh = args->flags & BB ? .99 : .95;

    // GAME TIME!!!    
    int board_height = screen_height() / CELL_SIZE;
    int board_width = screen_width() / CELL_SIZE;

    // Set up the board with random start
    int* current_pattern = (*gen_random)(board_width, board_height, 20);
    if (args->flags & CLEAR) {
        memset(current_pattern, 0, board_width * board_height * sizeof(int));
    }

    // track how many dead there are
    float dead = 0;
    const float total = board_width * board_height;

    // define the colors as ints for X11
    int dead_color_int = rgb_to_int(args->dead_color);
    int alive_color_int = rgb_to_int(args->alive_color);
    int dying_color_int = rgb_to_int(args->dying_color);

    // set the color to the background color
    int cur_color = dead_color_int;
    color(args->dead_color);

// Main loop
DWORD start_time = GetTickCount();
int frame_duration = 1000 / args->framerate;  // Frame duration based on the framerate

while (1) {
    DWORD frame_start = GetTickCount();  // Start of the frame

    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (msg.message == WM_QUIT) {
        break;
    }

    if (paused){
        continue;       //Skip the drawing loop if paused
    }

    if (add_mode){
        if (is_lmb_pressed()) {
            //set the color to the alive color
            color(args->alive_color);
            // get the mouse position
            POINT mouse_pos = get_mouse_pos();
            int x = mouse_pos.x / CELL_SIZE;
            int y = mouse_pos.y / CELL_SIZE;

            // fill the cell
            current_pattern[y * board_width + x] = ALIVE;
            fill_func(x, y, CELL_SIZE);
            }

        continue;       //Skip the drawing loop if in add mode
    }


    /* DRAWING PORTION */
    for (int i = 0; i < board_width * board_height; i++) {
        // Check and set the current color based on cell state
        if (current_pattern[i] == ALIVE && cur_color != alive_color_int) {
            cur_color = alive_color_int;
            color(args->alive_color);
        } else if (current_pattern[i] == DYING && cur_color != rgb_to_int(args->dying_color)) {
            cur_color = rgb_to_int(args->dying_color);
            color(args->dying_color);
        } else if (current_pattern[i] == DEAD) {
            dead++;
            if(cur_color != dead_color_int){
                cur_color = dead_color_int;
                color(args->dead_color);
            }
        }

        // Fill the cell
        (*fill_func)(i % board_width, i / board_width, CELL_SIZE);
    }

    // Fill the border cells (ensure full screen coverage)
    color(args->dead_color);
    cur_color = dead_color_int;
    for (int i = 0; i < board_width; i++) {
        (*fill_func)(i, board_height, CELL_SIZE);
    }
    for (int i = 0; i < board_height; i++) {
        (*fill_func)(board_width, i, CELL_SIZE);
    }

    /* GENERATION PORTION */
    int* next_pattern = (*gen_next)(current_pattern, board_width, board_height);
    free(current_pattern);
    current_pattern = next_pattern;


    if (dead / total >= restock_thresh && !(args->flags & NO_RESTOCK)) {
        add_random(current_pattern, board_width, board_height, 10);
    }
    dead = 0;


    // Frame timing logic
    DWORD frame_end = GetTickCount();
    DWORD elapsed_time = frame_end - frame_start;
    if (elapsed_time < frame_duration) {
        Sleep(frame_duration - elapsed_time);
    }
}

free(args);
free(current_pattern);
cleanup();
return 0;
}

#endif // SIMWALL_C