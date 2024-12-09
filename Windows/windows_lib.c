/*
Author: Pierce Lane, Chase Horner
Class: EECS 581
Assignment: Project 3
Creation Date: 10/21/2024

windows_lib.c 
Library file for the game of life displaying
Will basically have some Windows API helpers that will be used in the main file
*/

#ifndef WINDOWS_LIB
#define WINDOWS_LIB

#include "windows_lib.h"

#define NAME "SimWall"

// Globals for display information
static HWND hwnd;
static HDC hdc;

// Global variables for the program
bool paused = false;
bool add_mode = false;
bool clear = false;

// Global variables for the graphics
static uint32_t cur_color; //current color
static DWORD* pixels; //pointer to pixel buffer
static SIZE size; //size of window
static POINT ptSrc; //source point
static BLENDFUNCTION blend; //blend function for transparency
static HDC hMemDC; //for off-screen bitmap
static int pix_width; //width of screen in pixels
static int pix_height; //height of screen in pixels
static HBITMAP hBitmap; //for layered window updates

/* Functions */

/*
    Function to fill a cell at a given (x,y) coordinate with current color
*/
void fill_cell(int x, int y, size_t size) {
    for (int i = 0; i < size; ++i) { //loop through the height of the cell
        for (int j = 0; j < size; ++j) { //loop through the width of the cell
            pixels[(y*size + i) * screen_width() + (x*size + j)] = cur_color; //set color
        }
    }
}

/*
    Function to fill circle at a given (x,y) with current color
*/
void fill_circle(int x, int y, size_t size) {
    int radius = size / 2; //calculate radius of circle
    int center_x = x * size + radius; //calculate center x position of circle
    int center_y = y * size + radius; //calculate center y position of circle

    for (int i = 0; i < size; ++i) { //loop through height of circle
        for (int j = 0; j < size; ++j) { //loop through width of circle
            int dx = center_x - (x*size + j); //horizontal distance from point to center
            int dy = center_y - (y*size + i); //vertical distance from point to center
            if (dx*dx + dy*dy <= radius*radius) { //if point is within boundaries
                pixels[(y*size + i) * screen_width() + (x*size + j)] = cur_color; //set color
            }
        }
    }
}

/*
    Function that returns width of screen
*/
int screen_width(){
    return pix_width;
}

/*
    Function that returns height of screen
*/
int screen_height() {
    return pix_height;
}

/*
    Function to set foreground paint color using RGB struct
*/
void color(RGBA rgba) {
    if (rgba.r > rgba.a) { rgba.r = rgba.a; } //ensure red does not exceed alpha
    if (rgba.g > rgba.a) { rgba.g = rgba.a; } //ensure green does not exceed alpha
    if (rgba.b > rgba.a) { rgba.b = rgba.a; } //ensure blue does not exceed alphs
    cur_color = (rgba.a << 24) | (rgba.r << 16) | (rgba.g << 8) | rgba.b; //combine alpha and RGB into a single 32-bit integer
}

/*From https://stackoverflow.com/questions/5404277/c-win32-how-to-get-the-window-handle-of-the-window-that-has-focus
This is the process to put a window behind the desktop icons
There exists a window called WorkerW that is the parent of the desktop icons, so we can attach our window to that window
But there are multiple WorkerW windows, so we need to find the correct one
So we look for the WorkerW window that is the parent of the SHELLDLL_DefView window, which is the window that contains the desktop icons
*/

// Declare the global variable for WorkerW
HWND hWorkerW = NULL;

/*
    Callback function for EnumWindows in C
*/
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    HWND hShellView = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
    if (hShellView != NULL) {
        hWorkerW = FindWindowEx(NULL, hwnd, "WorkerW", NULL);
        return FALSE; 
    }
    return TRUE;
}

HWND GetDesktopWorkerW() {
    // Send a message to Progman to spawn WorkerW
    HWND hProgman = FindWindow("Progman", NULL); //get program window handle
    SendMessageTimeout(hProgman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL); //send message to spawn WorkerW

    // Enumerate through windows to find the WorkerW
    EnumWindows(EnumWindowsProc, 0); //call EnumWindows with callback function

    return hWorkerW; //return WorkerW handle
}

void Detach(HWND win) {
    // Remove the parent from our window
    SetParent(win, NULL); //set parent of window to NULL
}

/*
    Function clean everything up
*/
void cleanup() {
    // Hide and destroy main window
    ShowWindow(hwnd, SW_HIDE); //hide application window
    ShowWindow(hWorkerW, SW_HIDE); //hide WorkerW window
    ReleaseDC(hwnd, hdc); //release device context
    Detach(hwnd); //detach the application window from parent
    DestroyWindow(hwnd); //destroy application window
    
    // Unregister the hotkeys
    UnregisterHotKey(NULL, 1); //unregister hotkey with id = 1
    UnregisterHotKey(NULL, 2); //unregister hotkey with id = 2

    // Release transparency related stuff
    DeleteDC(hMemDC); //delete memory device context
    ReleaseDC(hwnd, hdc); //release device context for window
}

POINT get_mouse_pos() {
    /* Returns the current mouse position relative to the window */
    POINT point;
    if (GetCursorPos(&point)) { //get cursor position
        if (ScreenToClient(hwnd, &point)) { //convert screen coordinates to client window coordinates
            return point; //return mouse position
        }
    }
    POINT invalid_point = {-1, -1}; //if the position cannot be determined
    return invalid_point; //return invalid point
}

bool is_lmb_pressed() {
    /* Returns true if the left mouse button is pressed */
    POINT point;
    if (GetCursorPos(&point)) { //get cursor position
        if (ScreenToClient(hwnd, &point)) { //convert screen coordinates to client window coordinates
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) { //if left mouse button is pressed
                return true; //return true
            }
        }
    }
    return false; //otherwise return false
}

//Custom WNProc to handle hotkeys
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        // Handle the hotkey messages
       case WM_HOTKEY: 
        switch (wParam) {
            case 1:  // Ctrl+Alt+Q  Post a quit message to break loop in simwall.c
                PostQuitMessage(0); //send quit message
                break;

            case 2:  // Ctrl+Alt+P
                paused = !paused; //toggle paused state
                break;

            case 3:  // Ctrl+Alt+A
                add_mode = !add_mode; //toggle add mode
                break;

            case 4:  // Ctrl+Alt+D
                clear = true; //set clear flat to true
                break;
        }
        //If not a hotkey message, pass it to the default handler
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam); //call default window procedure
    }
    return 0;
}

void update_window() {
    UpdateLayeredWindow(hwnd, hdc, NULL, &size, hMemDC, &ptSrc, 0, &blend, ULW_ALPHA);
    //update layered window with current pixel data with blend function for transparency
}

// Sets up a window with the specified background color
HWND window_setup(RGBA bg_color) {
    WNDCLASS wc = {0};
    wc.lpszClassName = NAME;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WndProc; 
    wc.hbrBackground = CreateSolidBrush(RGB(bg_color.r, bg_color.g, bg_color.b));
    //solid brush with specified background color

    // set screen width and height
    pix_width = GetSystemMetrics(SM_CXSCREEN) * 3/2; //calculate screen width
    pix_height = GetSystemMetrics(SM_CYSCREEN) * 3/2; //calculate screen height

    // Register the window class
    if (!RegisterClass(&wc)) { //if window class registration fails
        MessageBox(NULL, "Failed to register window class", "Error", MB_ICONERROR); //error message
        exit(1); //exit program
    }

    // Create the window
    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, // Extended styles
        NAME, "SimWall", WS_POPUP | WS_VISIBLE, // WS_POPUP for a borderless window
        0, 0, screen_width(), screen_height(), // Fullscreen dimensions
        NULL, NULL, wc.hInstance, NULL
    );

    // Check if the window was created successfully
    if (hwnd == NULL) {
        printf(NULL, "Failed to create window", "Error", MB_ICONERROR);
        exit(1); //exit program
    }

    // Get the WorkerW window and make sure it is visible
    HWND hWorkerW = GetDesktopWorkerW();
    ShowWindow(hWorkerW, SW_SHOW);      


    // Attach  window to the WorkerW window using SetParent
    SetParent(hwnd, hWorkerW);

    // Show the window and make it the bottommost window
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOACTIVATE | SWP_NOMOVE);

    // Initialize the graphics context
    hdc = GetDC(hwnd);

    // Transparency stuff
    // Create a compatible bitmap with alpha
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = screen_width();
    bmi.bmiHeader.biHeight = -screen_height(); // Negative height for top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32 bits per pixel
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits;
    hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    if (!hBitmap) {
        MessageBox(NULL, "Failed to create DIB section!", "Error", MB_ICONERROR | MB_OK);
        exit(1);
    }

    // Fill the bitmap with semi-transparent red
    hMemDC = CreateCompatibleDC(hdc);
    SelectObject(hMemDC, hBitmap);

    pixels = (DWORD*)pvBits;

    // Set up the window to support per-pixel alpha
    ptSrc = (POINT){0, 0};
    size = (SIZE){screen_width(), screen_height()};
    blend = (BLENDFUNCTION){AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    // fill the whole window with the background color given
    color(bg_color);
    for (int i = 0; i < screen_width() * screen_height(); ++i) {
        pixels[i] = cur_color;
    }

    // Update the window
    update_window();

    // Show the window
    ShowWindow(hwnd, SW_SHOW);

    return hwnd;
}

#endif // WINDOWS_LIB
