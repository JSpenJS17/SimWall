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

#include <stdio.h> //standard I/O library
#include <stdlib.h> //standard memory allocation library
#include <stdbool.h> //Boolean type support
#include <windows.h> //Window specific library for system-level operations
#include <string.h> //string manipulation
#include <time.h> //time manipulation

#include "windows_lib.h" //custom library for Windows
#include "game_of_life/game_of_life.h" //Game of Life (GoL) library
#include "brians_brain/brians_brain.h" //Brian's Brain library
#include "seeds/seeds.h" //Seeds library
#include "langtons_ant/langtons_ant.h" //Langton's Ant library

//flags
#define DAEMONIZE   1
#define CIRCLE      (1 << 1)
#define KEYBINDS    (1 << 2)
#define BB          (1 << 3)
#define CLEAR       (1 << 4)
#define NO_RESTOCK  (1 << 5)
#define SEEDS       (1 << 6)
#define ANT         (1 << 7)


/* General purpose cmd-line args*/
typedef struct Args {
    RGBA alive_color, dead_color, dying_color, ant_color; //color states for simulations
    unsigned char flags; //flags for simulation options
    Ant* ants; //pointer to array of Langton's Ants
    int num_ants; //variable integer for number of ants in Langton's Ant
    float framerate; //variable float for frame rate for simulation
} Args;

/* Struct to store board information */
typedef struct Board {
    int width, height; //variable integers for dimensions of simulation
    int* pattern; //pointer to array for the board pattern
} Board;

// Global variables
int CELL_SIZE = 25; //default cell size
void (*fill_func)(int, int, size_t); // x, y, size
Args* args; //pointer to command line argument structure above
int cur_color; //variable to track current color
int cur_color; 
RGBA* color_list; //array of colors for Langton's Ant

// Langton's Ant specific globals
size_t num_colors; //number of colors in color list
char ruleset[128]; //null-terminated string of rules, max 128 chars

//Function to display usage instructions for SimWall
void usage() {
    fprintf(stderr, "Usage: simwall [options]\n"); //print usage header
    fprintf(stderr, "Options:\n"); //header for options
    fprintf(stderr, "  -h, --help: Show this help message\n"); //option for help
    fprintf(stderr, "  -D, -d, --daemonize: Daemonize the process\n"); //option to daemonize
    fprintf(stderr, "  -dead 000000FF: Set the dead cell color (RGBA)\n"); //set dead cell color
    fprintf(stderr, "  -alive FFFFFFFF: Set the alive cell color (RGBA)\n"); //set alive cell color
    fprintf(stderr, "  -dying 808080FF: Set the dying cell color (RGBA)\n"); //set dying cell color
    fprintf(stderr, "  -fps 10.0: Set the framerate\n"); //set framerate
    fprintf(stderr, "  -bb: Run Brian's Brain (BB) instead of Game of Life\n"); //switch to Brian's Brain
    fprintf(stderr, "  -seeds: Run Seeds instead of Game of Life\n"); //switch to Seeds
    fprintf(stderr, "  -ant <ant_params.txt>: Run Langton's Ant instead of Game of Life.\n"); //switch to Langton's Ant
    fprintf(stderr, "                         Ant parameters are optional.\n");
    fprintf(stderr, "    -ant_params.txt: Give ant parameters in a file.\n"); //ant parameter file details
    fprintf(stderr, "       Format:\n");
    fprintf(stderr, "         RULESET\n");
    fprintf(stderr, "         CELL COLOR LIST (RGBA values, space delimited)\n");
    fprintf(stderr, "         X0 Y0 START_DIRECTION ANT0_COLOR\n");
    fprintf(stderr, "         X1 Y1 START_DIRECTION ANT1_COLOR\n");
    fprintf(stderr, "         etc...\n");
    fprintf(stderr, "       For direction, 0:UP, 1:RIGHT, 2:DOWN, 3:LEFT\n"); // might change to strings that are parsed to the enum later
    fprintf(stderr, "       For ruleset, R:RIGHT, L:LEFT, C:CONTINUE, U:U-TURN (ex. RLLRCU)\n");
    fprintf(stderr, "       Cell color list length must be >= to ruleset length and\n");
    fprintf(stderr, "         can be set to default values by providing the keyword \"default\"\n");
    fprintf(stderr, "         or \"default_alpha\" for a transparent background\n");
    fprintf(stderr, "  -c: Draw circles instead of a squares\n"); //option to switch from default squares to circles
    fprintf(stderr, "  -s 25: Set the cell size in pixels\n"); //option to set cell size
    fprintf(stderr, "  -nk: Disable keybinds\n"); //option to disable keybinds
    fprintf(stderr, "  -nr: No restocking if board is too empty\n"); //option to disable restocking
    fprintf(stderr, "  -clear: Start with a clear board. Includes -nr\n"); //option to clear board
    fprintf(stderr, "Example: simwall -dead FF00FFFF -alive FFFF00FF -fps 7.5\n"); //example usage
    exit(1);
}

/*
    Count number of lines in file
*/
int count_lines(const char* filename) {
    FILE *file = fopen(filename, "r"); //open file in read mode
    if (file == NULL) { //if file cannot be opened
        perror("Failed to open file"); //print error message
        return -1; //indicate error
    }

    int line_count = 0; //variable for line count
    char ch; //variable for character inf ile

    while ((ch = fgetc(file)) != EOF) { //read file character by character
        if (ch == '\n') {
            line_count++; //increment line counter if newline counter seen
        }
    }

    fclose(file); //close file
    return line_count; //return total number of lines
}

void parse_ants_file(FILE* ants_file, const char* filename) {
    // read the ruleset
    if (fgets(ruleset, 128, ants_file) == NULL) {
        fprintf(stderr, "Failed to read from file or EOF reached.\n");
        usage();
    }
    ruleset[strlen(ruleset) - 1] = '\0'; // Ensure null-terminated, also remove newline

    // read the color list line
    char color_list_str[2048]; // 2048 is big enough for 128 * 9 (8 hex + 1 space)
    if (fgets(color_list_str, 2048, ants_file) == NULL) {
        fprintf(stderr, "Failed to read from file or EOF reached.\n");
        usage();
    } 
    color_list_str[strlen(color_list_str)-1] = '\0'; // remove the newline
    
    // parse the color list
    char* color_str = strtok(color_list_str, " ");

    // how many colors there SHOULD be
    num_colors = strlen(ruleset);
    // allocate space for the color list
    color_list = (RGBA*)malloc(num_colors * sizeof(RGBA));

    // check if it should be default colors
    bool use_defaults = false;
    bool use_alpha = false;
    // check if they wanted alpha background
    if (strcmp(color_str, "default_alpha") == 0) {
        use_alpha = true;
    }

    // then, if they wanted default colors at all
    if (strcmp(color_str, "default") == 0 || use_alpha) {
        // default colors
        use_defaults = true;
        // define step size to be equal steps between 0 and 255
            // num_colors-1 ensure background is black and fully on is white
        int step_size = 255 / (num_colors-1);
        for (int j = 0; j < num_colors; j++) {
            // otherwise, set the color to be a shade of gray
            color_list[j] = (RGBA){j*step_size, j*step_size, j*step_size, 255};
        }
        if (use_alpha) {
            // set the background color to be transparent instead of black
            color_list[0] = (RGBA){0, 0, 0, 0};   
        }
    }


    // if we're not using the default colors, i.e. they gave us some
    if (!use_defaults) {
        // if there are too many colors, skip em!
        for (int j = 0; j < num_colors; j++) {
            // check for errors
            if (color_str == NULL) { // not enough colors
                fprintf(stderr, "Color list too short\n");
                usage();
            }
            if (strlen(color_str) != 8) { // not a valid color
                fprintf(stderr, "Invalid color: %s\n", color_str);
                usage();
            }

            // convert the hex to RGBA
            sscanf(color_str, "%2hx%2hx%2hx%2hx", &color_list[j].r, &color_list[j].g, &color_list[j].b, &color_list[j].a);
            // get the next color
            // not 100% on why the NULL is needed here instead of color_list_str, but it is
            color_str = strtok(NULL, " ");
        }
    }

    // set dead color because it's used as window background
    args->dead_color = color_list[0];

    // read the number of ants
    args->num_ants = count_lines(filename)-2;
    if (args->num_ants <= 0) {
        fprintf(stderr, "Invalid ant file: %s\n", filename);
        usage();
    }

    // allocate space for the ants
    args->ants = (Ant*)malloc(args->num_ants * sizeof(Ant));
    // read the ants
    for (int j = 0; j < args->num_ants; j++) {
        Ant ant;
        char line[256];
        // read in a line
        if (fgets(line, 256, ants_file) == NULL) {
            fprintf(stderr, "Failed to read from file or EOF reached.\n");
            usage();
        }
        // parse the line
        int num = sscanf(line, "%d %d %d %2hx%2hx%2hx%2hx\n",
                    &ant.x, &ant.y, (int*)&ant.direction,
                    &ant.color.r, &ant.color.g, &ant.color.b, &ant.color.a);
        // check for errors (lightly, not a full check)
        if (num != 7) {
            fprintf(stderr, "Invalid ant line: %s\n", line);
            usage();
        }
        // add the ant to the list
        args->ants[j] = ant;
    }


}

void parse_args(int argc, char **argv) {
    /* Parses the args for the program.
    Takes argc, argv from cmdline.
    !! FREE THE RETURNED ARGS !! */
    // define defaults
    args = malloc(sizeof(Args));
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
            // convert the hex to RGB
            char* dead_color_str = argv[i+1];
            if (strlen(dead_color_str) != 8) {
                fprintf(stderr, "Invalid color: %s\n", dead_color_str);
                usage();
            }
            // Use sscanf to convert hex to RGBA
            sscanf(dead_color_str, "%2hx%2hx%2hx%2hx", &args->dead_color.r, &args->dead_color.g, &args->dead_color.b, &args->dead_color.a);
            i++; // increment i to simulate parsing the hex string
        }
        // dying color
        else if (strcmp(argv[i], "-dying") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -dying\n");
                usage();
            }
            // convert the hex to RGBA
            char* dying_color_str = argv[i+1];
            if (strlen(dying_color_str) != 8) {
                fprintf(stderr, "Invalid color: %s\n", dying_color_str);
                usage();
            }
            // Use sscanf to convert hex to RGBA
            sscanf(dying_color_str, "%2hx%2hx%2hx%2hx", &args->dying_color.r, &args->dying_color.g, &args->dying_color.b, &args->dying_color.a);
            i++; // increment i to simulate parsing the hex string
        }
        // alive color
        else if (strcmp(argv[i], "-alive") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Not enough arguments for -alive\n");
                usage();
            }
            // convert the hex to RGBA
            char* alive_color_str = argv[i+1];
            if (strlen(alive_color_str) != 8) {
                fprintf(stderr, "Invalid color: %s\n", alive_color_str);
                usage();
            }
            // Use sscanf to convert hex to RGBA
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
        
            // check if next argument exists or if the next arg is a flag
            if (i + 1 >= argc || argv[i+1][0] == '-') {
                // if so, use default ant and we're done
                continue;
            }

            // otherwise, we have an ant file to parse
            // open the file
            FILE* ants_file = fopen(argv[i+1], "r");
            if (!ants_file) {
                fprintf(stderr, "Could not open file: %s\n", argv[i+1]);
                usage();
            }
            
            // parse the file
            parse_ants_file(ants_file, argv[i+1]);

            // remember to close the file
            fclose(ants_file);
            i += 1;
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
}

void simwall_cleanup() {
    free(color_list);
    free(args->ants);
    free(args);
}

void sigint_handler(int sig) {
    // Cleanup and exit
    simwall_cleanup();
    cleanup();
    exit(0);
}

int main(int argc, char **argv) {
    // Set up the signal interrupt (ctrl+c) handler
    signal(SIGINT, sigint_handler);

    // parse arguments
    parse_args(argc, argv);

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

   if (args->flags & ANT) {
        // do ant things
        if (!args->ants) {
            // Default ant
            args->num_ants = 1;
            args->ants = (Ant*)malloc(sizeof(Ant));
            args->ants[0].x = cur_board.width / 2;
            args->ants[0].y = cur_board.height / 2;
            args->ants[0].direction = UP;
            args->ants[0].color = (RGBA){255, 0, 0, 255};

            // Default color list
            color_list = (RGBA*)malloc(2 * sizeof(RGBA));
            color_list[0] = (RGBA){0, 0, 0, 255};
            color_list[1] = (RGBA){255, 255, 255, 255};
            num_colors = 2;

            // Default ruleset
            ruleset[0] = 'R';
            ruleset[1] = 'L';
            ruleset[2] = '\0';
        }
        // Initialize the ants
        init_ants(args->ants, args->num_ants, ruleset);
    } else {
        num_colors = 3;
        color_list = (RGBA*)malloc(3 * sizeof(RGBA));
        color_list[0] = args->dead_color;
        color_list[1] = args->alive_color;
        color_list[2] = args->dying_color;
    }


    // set the color to the background color
    cur_color = DEAD;    
    color(color_list[cur_color]);

    // define iter count
    int iter_count = 0;

    // Main loop
    int frame_duration = 1000 / args->framerate;  // Frame duration based on the framerate

    // printf("Your screen is %d cells wide and %d cells tall\n", cur_board.width, cur_board.height);
    while (1) {
        DWORD frame_start = GetTickCount();  // Start of the frame

        //Handle Hot keys
        MSG msg;
        //Check for messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //Quit when Ctrl+Alt+Q is pressed
        if (msg.message == WM_QUIT) {
            break;
        }

        //Pause when Ctrl+Alt+P is pressed
        if (paused){
            continue;       //Skip the drawing loop if paused
        }

        //Enter add mode when Ctrl+Alt+A is pressed and ants is not active
        if (add_mode && !(args->flags & ANT)){
            if (is_lmb_pressed()) {
                //set the color to the alive color
                color(args->alive_color);
                cur_color = ALIVE;
                // get the mouse position
                POINT mouse_pos = get_mouse_pos();
                int x = mouse_pos.x / CELL_SIZE;
                int y = mouse_pos.y / CELL_SIZE;

                // fill the cell
                cur_board.pattern[y * cur_board.width + x] = ALIVE;
                fill_func(x, y, CELL_SIZE);
                update_window();
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
                fill_func(i % cur_board.width, i / cur_board.width, CELL_SIZE);
            }
        }

        /* DRAWING PORTION */
        for (int i = 0; i < cur_board.height * cur_board.width; i++) {
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
            fill_func(i % cur_board.width, i / cur_board.width, CELL_SIZE);
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

        // Update the window to push frame changes
        update_window();

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
            if (dead/total >= restock_thresh && !(args->flags & NO_RESTOCK) && clear == false) {
                add_random(cur_board.pattern, cur_board.width, cur_board.height, 20);
                iter_count = 0;            }
        }
        // reset dead count
        dead = 0;
        clear = false;


        // Frame timing logic
        DWORD frame_end = GetTickCount();
        DWORD elapsed_time = frame_end - frame_start;
        if (elapsed_time < frame_duration) {
            Sleep(frame_duration - elapsed_time);
        }

    }

    // cleanup
    free(cur_board.pattern);
    simwall_cleanup();
    cleanup();
    return 0;
}

#endif // SIMWALL_C
