/* windows_lib.c 
Library file for the game of life displaying
Will basically have some Windows API helpers that will be used in the main file
*/
#ifndef WINDOWS_LIB
#define WINDOWS_LIB

#include "windows_lib.h"

// Helpful macros and globals
#define OPAQUE 0xffffffff

#define NAME "SimWall"
#define VERSION "0.1"

#define ATOM(a) XInternAtom(display, #a, False)

// Globals for display information
static HWND hwnd;
static HDC hdc;

// Graphics context for drawing
static HPEN hPen;
static HBRUSH hBrush;

/* Functions */
void fill_cell(int x, int y, int size) {
    /* Fills a cell at x, y with the current color */
    RECT rect = {x*size, y*size, (x+1)*size, (y+1)*size};
    FillRect(hdc, &rect, hBrush);
}

void fill_circle(int x, int y, int size) {
    /* Fills a circle at x, y with the current color */
    Ellipse(hdc, x*size, y*size, (x+1)*size, (y+1)*size);
    }

    // Get the screen width and height
int screen_width(){
    return GetSystemMetrics(SM_CXSCREEN);
}

int screen_height() {
    return GetSystemMetrics(SM_CYSCREEN);
}

int rgb_to_int(RGB rgb) {
    /* Converts an RGB struct to an int for X11 compatibility */
    return rgb.r << 16 | rgb.g << 8 | rgb.b;
}

void color(RGB rgb) {
    /* Sets foreground paint color using RGB struct */
    
    // Change the pen color
    SetDCPenColor(hdc, RGB(rgb.r, rgb.g, rgb.b));
    
    // Change the brush color
    if (hBrush) {
        DeleteObject(hBrush);
    }
    hBrush = CreateSolidBrush(RGB(rgb.r, rgb.g, rgb.b));
    SelectObject(hdc, hBrush);

}

//From https://stackoverflow.com/questions/5404277/c-win32-how-to-get-the-window-handle-of-the-window-that-has-focus
// Message to `Progman` to spawn a `WorkerW`
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
    SendMessageTimeout(hProgman, 0x052C, NULL, NULL, SMTO_NORMAL, 1000, NULL);

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
    DeleteObject(hPen);
    DeleteObject(hBrush);
    Detach(hwnd);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
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

bool check_for_keybind(char* key) {
    /* Checks for the keybind and returns true if one is found */
    SHORT keyState = GetAsyncKeyState((int)key[0]);
    if (keyState & 0x8000) {
        return true;
    }
    return false;
}

bool wait_for_keybind(char* key) {
    /* Waits for the keybind and returns true if one is found */
    while(true) {
        if (check_for_keybind(key)) {
            return true;
        }
    }
}

HWND get_window() {
    /* Returns the window */
    return hwnd;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}



HWND window_setup(RGB bg_color) {
    WNDCLASS wc = {0};
    wc.lpszClassName = NAME;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WndProc; 
    wc.hbrBackground = CreateSolidBrush(RGB(bg_color.r, bg_color.g, bg_color.b));

    // Register the window class
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Failed to register window class", "Error", MB_ICONERROR);
        exit(1);
    }

    // Create the window
    hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, // Extended styles
        NAME, "SimWall", WS_POPUP | WS_VISIBLE, // WS_POPUP for a borderless window
        0, 0, screen_width(), screen_height(), // Fullscreen dimensions
        NULL, NULL, wc.hInstance, NULL
    );

    if (hwnd == NULL) {
        printf(NULL, "Failed to create window", "Error", MB_ICONERROR);
        exit(1);
    }


    HWND hWorkerW = GetDesktopWorkerW();

    // Step 3: Attach your window to the WorkerW window using SetParent
    SetParent(hwnd, hWorkerW);

    // Show the window and make it the bottommost window
      ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    // Initialize the graphics context
    hdc = GetDC(hwnd);
    
    //Set up the pen and brush
    hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    hBrush = CreateSolidBrush(RGB(0, 0, 0)); // Default brush color
    SelectObject(hdc, hPen);
    SelectObject(hdc, hBrush);


    return hwnd;
}

#endif // WINDOWS_LIB