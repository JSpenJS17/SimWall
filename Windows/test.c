#include <windows.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>

// Window procedure declaration
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

DWORD* pixels;
const size_t CELL_SIZE = 25;
const size_t WIDTH = 800;
const size_t HEIGHT = 600;
uint32_t cur_color = 0xFFFFFFFF; // ARGB: Solid white

typedef unsigned short ushort;

typedef struct ARGB {
    ushort a, r, g, b;
} ARGB;

void fill_cell(int x, int y, size_t cell_size) {
    /* Fills a cell at x, y with the current color */
    for (int i = 0; i < cell_size; ++i) {
        for (int j = 0; j < cell_size; ++j) {
            pixels[(y * CELL_SIZE + i) * WIDTH + (x * CELL_SIZE + j)] = cur_color;
        }
    }
}

void color(ARGB rgba) {
    /* Sets paint color using ARGB struct */
    cur_color = (rgba.a << 24) | (rgba.r << 16) | (rgba.g << 8) | rgba.b;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Registering the window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RGBAWindowClass";

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Failed to register window class!", "Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Create the window
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED,                // Extended window style for layered window
        "RGBAWindowClass",            // Window class name
        "RGBA Window",                // Window title
        WS_POPUP,                     // Style (no borders, title bar, etc.)
        0, 0, WIDTH, HEIGHT,           // Position and size
        NULL, NULL, hInstance, NULL   // Parent, menu, instance, additional params
    );

    if (!hwnd) {
        MessageBox(NULL, "Failed to create window!", "Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Set up the window to support per-pixel alpha
    HDC hdc = GetDC(hwnd);

    // Create a compatible bitmap with alpha
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = WIDTH;
    bmi.bmiHeader.biHeight = -HEIGHT; // Negative height for top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32 bits per pixel
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    if (!hBitmap) {
        MessageBox(NULL, "Failed to create DIB section!", "Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Fill the bitmap with semi-transparent red
    HDC hMemDC = CreateCompatibleDC(hdc);
    SelectObject(hMemDC, hBitmap);

    pixels = (DWORD*)pvBits;
    
    // Find dimnensions in terms of # of cells
    int board_width = WIDTH / CELL_SIZE;
    int board_height = HEIGHT / CELL_SIZE;

    for (int y = 0; y < board_height; ++y) {
        for (int x = 0; x < board_width; ++x) {
            if (x == y) {
                color((ARGB){0, 0, 0, 0}); // transparent
            } else {
                color((ARGB){255, 255, 255, 255}); // Solid white
            }
            fill_cell(x, y, CELL_SIZE);
        }
    }

    POINT ptSrc = {0, 0};
    SIZE size = {800, 600};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, hdc, NULL, &size, hMemDC, &ptSrc, 0, &blend, ULW_ALPHA);

    // Show the window
    ShowWindow(hwnd, nCmdShow);

    Sleep(1000);

    printf("Changing color to red\n"); fflush(stdout);

    color((ARGB){128, 255, 0, 0}); // RED
    for (int y = 0; y < board_height; ++y) {
        for (int x = 0; x < board_width; ++x) {
            fill_cell(x, y, CELL_SIZE);
        }
    }

    UpdateLayeredWindow(hwnd, hdc, NULL, &size, hMemDC, &ptSrc, 0, &blend, ULW_ALPHA);

    DeleteDC(hMemDC);
    ReleaseDC(hwnd, hdc);

    while (1) {
        Sleep(1000);
    }

    return 0;
}

// Window procedure implementation
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
