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
void fill_cell(int x, int y, size_t size) {
    /* Fills a cell at x, y with the current color */
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            pixels[(y*size + i) * screen_width() + (x*size + j)] = cur_color;
        }
    }
}

void fill_circle(int x, int y, size_t size) {
    /* Fills a circle at x, y with the current color */
    int radius = size / 2;
    int center_x = x * size + radius;
    int center_y = y * size + radius;

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int dx = center_x - (x*size + j);
            int dy = center_y - (y*size + i);
            if (dx*dx + dy*dy <= radius*radius) {
                pixels[(y*size + i) * screen_width() + (x*size + j)] = cur_color;
            }
        }
    }
}

// Get the screen width and height
int screen_width(){
    return pix_width;
}

int screen_height() {
    return pix_height;
}

void color(RGBA rgba) {
    /* Sets foreground paint color using RGB struct */
    if (rgba.r > rgba.a) { rgba.r = rgba.a; }
    if (rgba.g > rgba.a) { rgba.g = rgba.a; }
    if (rgba.b > rgba.a) { rgba.b = rgba.a; }
    cur_color = (rgba.a << 24) | (rgba.r << 16) | (rgba.g << 8) | rgba.b;
}

/*From https://stackoverflow.com/questions/5404277/c-win32-how-to-get-the-window-handle-of-the-window-that-has-focus
This is the process to put a window behind the desktop icons
There exists a window called WorkerW that is the parent of the desktop icons, so we can attach our window to that window
But there are multiple WorkerW windows, so we need to find the correct one
So we look for the WorkerW window that is the parent of the SHELLDLL_DefView window, which is the window that contains the desktop icons
*/

// Declare the global variable for WorkerW
HWND hWorkerW = NULL;

// Callback function for EnumWindows in C
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
    HWND hProgman = FindWindow("Progman", NULL);
    SendMessageTimeout(hProgman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);

    // Enumerate through windows to find the WorkerW
    EnumWindows(EnumWindowsProc, 0);


    return hWorkerW;
}

void Detach(HWND win) {
    // Remove the parent from our window
    SetParent(win, NULL);
}


void cleanup() {
    /* Cleans everything up, be sure to call when done */
    // Hide and destroy main window
    ShowWindow(hwnd, SW_HIDE);
    ShowWindow(hWorkerW, SW_HIDE);
    ReleaseDC(hwnd, hdc);
    Detach(hwnd);
    DestroyWindow(hwnd);
    
    // Unregister the hotkeys
    UnregisterHotKey(NULL, 1);
    UnregisterHotKey(NULL, 2);

    // Release transparency related stuff
    DeleteDC(hMemDC);
    ReleaseDC(hwnd, hdc);
}

POINT get_mouse_pos() {
    /* Returns the current mouse position relative to the window */
    POINT point;
    if (GetCursorPos(&point)) {
        if (ScreenToClient(hwnd, &point)) {
            return point;
        }
    }
    POINT invalid_point = {-1, -1}; // Return an invalid point if the position cannot be determined
    return invalid_point;
}

bool is_lmb_pressed() {
    /* Returns true if the left mouse button is pressed */
    POINT point;
    if (GetCursorPos(&point)) {
        if (ScreenToClient(hwnd, &point)) {
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                return true;
            }
        }
    }
    return false;
}

//Custom WNProc to handle hotkeys
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        // Handle the hotkey messages
       case WM_HOTKEY: 
        switch (wParam) {
            case 1:  // Ctrl+Alt+Q  Post a quit message to break loop in simwall.c
                PostQuitMessage(0);
                break;

            case 2:  // Ctrl+Alt+P
                paused = !paused;
                break;

            case 3:  // Ctrl+Alt+A
                add_mode = !add_mode;
                break;

            case 4:  // Ctrl+Alt+D
                clear = true;
                break;
        }
        //If not a hotkey message, pass it to the default handler
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void update_window() {
    UpdateLayeredWindow(hwnd, hdc, NULL, &size, hMemDC, &ptSrc, 0, &blend, ULW_ALPHA);
}

// Sets up a window with the specified background color
HWND window_setup(RGBA bg_color) {
    WNDCLASS wc = {0};
    wc.lpszClassName = NAME;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WndProc; 
    wc.hbrBackground = CreateSolidBrush(RGB(bg_color.r, bg_color.g, bg_color.b));

    // set screen width and height
    pix_width = GetSystemMetrics(SM_CXSCREEN) * 3/2;
    pix_height = GetSystemMetrics(SM_CYSCREEN) * 3/2;

    // Register the window class
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Failed to register window class", "Error", MB_ICONERROR);
        exit(1);
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
        exit(1);
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
