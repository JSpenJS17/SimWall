/*
Author: Pierce Lane, Chase Horner
Class: EECS 581
Assignment: Project 3
Creation Date: 10/21/2024

simwall.c
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
#include "seeds/seeds.h"
#include "langtons_ant/langtons_ant.h"

#define DAEMONIZE   1
#define CIRCLE      1 << 1
#define KEYBINDS    1 << 2
#define BB          1 << 3
#define CLEAR       1 << 4
#define NO_RESTOCK  1 << 5
#define SEEDS       1 << 6
#define ANT         1 << 7


/* General purpose cmd-line args 
Can add more later if needed */
struct Args {
    RGB alive_color, dead_color, dying_color, ant_color;
    unsigned char flags;
    char* ant_rules;
    float framerate;
};
typedef struct Args Args;

/* Struct to store board information */
struct Board {
    int width, height;
    int* pattern;
};
typedef struct Board Board;

// Globals
int CELL_SIZE = 25;
void (*fill_func)(int, int, size_t); // x, y, size
Args* args;
int cur_color;

void usage() {
    fprintf(stderr, "Usage: simwall [options]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help: Show this help message\n");
    fprintf(stderr, "  -D, -d, --daemonize: Daemonize the process\n");
    fprintf(stderr, "  -dead 000000: Set the dead cell color\n");
    fprintf(stderr, "  -alive FFFFFF: Set the alive cell color\n");
    fprintf(stderr, "  -dying 808080: Set the dying cell color\n");
    fprintf(stderr, "  -fps 10.0: Set the framerate\n");
    fprintf(stderr, "  -bb: Run Brian's Brain (BB) instead of Game of Life\n");
    fprintf(stderr, "  -seeds: Run Seeds instead of Game of Life\n");
    fprintf(stderr, "  -ant: Run Langton's Ant instead of Game of Life\n");
    fprintf(stderr, "    -ant_rules RLCU: Set the ant ruleset\n");
    fprintf(stderr, "    -ant_color FF0000: Set the ant color\n");
    //TODO: Implement these
    // fprintf(stderr, "    -color_list 000000 808080 FFFFFF ... : Set the color list for states in Langton's Ant")
    // fprintf(stderr, "    -ants ants.txt: Give input ant locations and directions in a file.\n       Format: x y direction\\n\n");
    fprintf(stderr, "  -c: Draw circles instead of a squares\n");
    fprintf(stderr, "  -s 25: Set the cell size in pixels\n");
    fprintf(stderr, "  -nk: Disable keybinds\n");
    fprintf(stderr, "  -nr: No restocking if board is too empty\n");
    fprintf(stderr, "  -clear: Start with a clear board. Includes -nr\n");
    fprintf(stderr, "Example: simwall -dead FF00FF -alive 00FF00 -fps 7.5\n");
    exit(1);
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
    args->ant_color.r = 255;
    args->ant_color.g = 0;
    args->ant_color.b = 0;
    args->ant_rules = "RL";

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
            // convert the hex to RGB
            char* dead_color_str = argv[i+1];
            if (strlen(dead_color_str) != 6) {
                fprintf(stderr, "Invalid color: %s\n", dead_color_str);
                usage();
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
            }
            // convert the hex to RGB
            char* dying_color_str = argv[i+1];
            if (strlen(dying_color_str) != 6) {
                fprintf(stderr, "Invalid color: %s\n", dying_color_str);
                usage();
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
            }
            // convert the hex to RGB
            char* alive_color_str = argv[i+1];
            if (strlen(alive_color_str) != 6) {
                fprintf(stderr, "Invalid color: %s\n", alive_color_str);
                usage();
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
        // langton's ant ruleset
        else if (strcmp(argv[i], "-ant_rules") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -ant_rules\n");
                usage();
            }
            args->ant_rules = argv[i+1];
            i += 1;
        }
        // langton's ant color
        else if (strcmp(argv[i], "-ant_color") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -ant_color\n");
                usage();
            }
            // convert the hex to RGB
            char* ant_color_str = argv[i+1];
            if (strlen(ant_color_str) != 6) {
                fprintf(stderr, "Invalid color: %s\n", ant_color_str);
                usage();
            }
            // Use sscanf to convert hex to RGB
            sscanf(ant_color_str, "%2hx%2hx%2hx", &args->ant_color.r, &args->ant_color.g, &args->ant_color.b);
            i++; // increment i to simulate parsing the hex string
        }
        // disable keybinds
        else if (strcmp(argv[i], "-nk") == 0) {
            args->flags &= ~KEYBINDS;
        }
        // brians brain
        else if (strcmp(argv[i], "-bb") == 0) {
            args->flags |= BB;
        }
        // seeds
        else if (strcmp(argv[i], "-seeds") == 0) {
            args->flags |= SEEDS;
        }
        // langton's ant
        else if (strcmp(argv[i], "-ant") == 0) {
            args->flags |= ANT;
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
int main(int argc, char **argv) {

    //Using mutex to prevent multiple instances of the program
    // Create a named mutex
    HANDLE hMutex = CreateMutex(NULL, TRUE, "SimWallMutex");
    if (hMutex == NULL) {
        fprintf(stderr, "Failed to create mutex\n");
        return 1;
    }

    // Check if the mutex already exists
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        fprintf(stderr, "Another instance of SimWall is already running\n");
        CloseHandle(hMutex);
        return 1;
    }

        // parse arguments
    args = parse_args(argc, argv);

    // daemonize if they asked for it
    //Doesnt work currently!
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
        if (!RegisterHotKey(hwnd, 4, MOD_CONTROL | MOD_ALT, 'D')) {
        printf("Failed to register hotkey for Ctrl+Alt+D\n");
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
    cur_board.height = screen_height() / CELL_SIZE;
    cur_board.width = screen_width() / CELL_SIZE;
    
    // Set up the board with random start
    cur_board.pattern = (*gen_random)(cur_board.width, cur_board.height, 20);
    if (args->flags & CLEAR) {
        memset(cur_board.pattern, 0, cur_board.width * cur_board.height * sizeof(int));
    }

    // track how many dead there are
    float dead = 0;
    const float total = cur_board.width * cur_board.height;

    // set up the ants
    // have to define these outside of the if
    int num_ants;
    Ant* ants;

    if (args->flags & ANT) {
        // instantiate ant-related values
        // WIP, need custom input .txt file implementation
        num_ants = 1;
        ants = (Ant*)malloc(num_ants * sizeof(Ant));
        for (int i = 0; i < num_ants; i++) {
            ants[i] = (Ant){.x = cur_board.width/2+i, .y = cur_board.height/2+i, .direction = 0};
        }
        init_ants(ants, 1, args->ant_rules);
    }

    // set the color to the background color
    int num_colors = 3;
    RGB color_list[] = {args->dead_color, 
                        args->alive_color, 
                        args->dying_color}; // can add more potentially
    color(color_list[cur_color]);
    cur_color = DEAD;    

    // define iter count
    int iter_count = 0;

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
            cur_board.pattern[y * cur_board.width + x] = ALIVE;
            fill_func(x, y, CELL_SIZE);
            }

        continue;       //Skip the drawing loop if in add mode
    }

    if (clear) {
        // Set the board to deads
        memset(cur_board.pattern, DEAD, cur_board.width * cur_board.height * sizeof(int));

        // Set the color
        color(args->dead_color);
        cur_color = DEAD;

        // Fill the board right here and now for instant updates!
        for (int i = 0; i < cur_board.width * cur_board.height; i++) {
            (*fill_func)(i % cur_board.width, i / cur_board.width, CELL_SIZE);
        }
        clear = false;
    }


    /* DRAWING PORTION */

        for (int i = 0; i < cur_board.width * cur_board.height; i++) {
            // if the color has changed
            if (cur_board.pattern[i] != cur_color) {
                // update color accordingly
                    // % num_colors to allow for ANT's variable number of states
                cur_color = cur_board.pattern[i] % num_colors; 
                color(color_list[cur_color]);
            }

            // increment dead counter if needed
            if (cur_color == DEAD) {
                dead++;
            }

            // fill the cell with whatever color we land on
            (*fill_func)(i % cur_board.width, i / cur_board.width, CELL_SIZE);
        }
        

        color(color_list[DEAD]);
        cur_color = DEAD;
        // fill one more row and col with bg to make sure we fill the whole screen
        for (int i = 0; i < cur_board.width; i++) {
            (*fill_func)(i, cur_board.height, CELL_SIZE);
        }
        for (int i = 0; i < cur_board.height; i++) {
            (*fill_func)(cur_board.width, i, CELL_SIZE);
        }

        // Handle drawing ants over the now completed board
        if (args->flags & ANT) {
            // Loop through the ants and draw them
            color(args->ant_color);
            cur_color = -1; // dummy value to let us know we need to reset the color
            for (int ant_index = 0; ant_index < num_ants; ant_index++) {
                Ant ant = ants[ant_index];
                fill_func(ant.x, ant.y, CELL_SIZE);
            }
        }

        /* GENERATION PORTION */
        // Now generate the next pattern
        int* next_pattern = (*gen_next)(cur_board.pattern, cur_board.width, cur_board.height);
        free(cur_board.pattern);
        cur_board.pattern = next_pattern;

        // increment iter count
        iter_count++;

        // check if we need to add more cells
        if (args->flags & SEEDS) {
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


    // Frame timing logic
    DWORD frame_end = GetTickCount();
    DWORD elapsed_time = frame_end - frame_start;
    if (elapsed_time < frame_duration) {
        Sleep(frame_duration - elapsed_time);
        }
    }

    // cleanup
    free(args);
    free(cur_board.pattern);
    cleanup();
    return 0;
}

#endif // SIMWALL_C