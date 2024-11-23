/* x11_lib.c 
Library file for the game of life displaying to X11
Will basically have some X11 helpers that will be used in the main file
*/
#ifndef X11_LIB
#define X11_LIB

#include "x11_lib.h" // our x11_lib.h file

// Helpful macros and globals
#define OPAQUE 0xffffffff

#define NAME "SimWall"
#define VERSION "0.1"

#define ATOM(a) XInternAtom(display, #a, False)

// Globals for display information
static Display *display = NULL;
static Window window;
static int screen;

// Graphics context for drawing
static GC gc;

/* Functions */
void fill_cell(int x, int y, size_t size) {
    /* Fills a cell at x, y with the current color */
    XFillRectangle(display, window, gc, x*size, y*size, size, size);
}

void fill_circle(int x, int y, size_t size) {
    /* Fills a circle at x, y with the current color */
    XFillArc(display, window, gc, x*size, y*size, size, size, 0, 360*64);
}

int screen_width() {
    /* Returns the width of the screen */
    return DisplayWidth(display, screen);
}

int screen_height() {
    /* Returns the height of the screen */
    return DisplayHeight(display, screen);
}

int argb_to_int(ARGB argb) {
    /* Converts an ARGB struct to an int for X11 compatibility */
    return argb.a << 24 | argb.r << 16 | argb.g << 8 | argb.b;
}

void color(ARGB argb) {
    /* Sets foreground paint color using ARGB struct */
    XSetForeground(display, gc, argb_to_int(argb));
}

void raise_window() {
    /* Raises the window to the top */
    XRaiseWindow(display, window);
}

void lower_window() {
    /* Lowers the window to the bottom */
    XLowerWindow(display, window);
}

void focus_window() {
    /* Focuses the window */
    XSetInputFocus(display, window, RevertToParent, CurrentTime);
}

void unfocus_window() {
    /* Unfocuses the window */
    XSetInputFocus(display, PointerRoot, RevertToParent, CurrentTime);
}

void flush() {
    /* Flushes the display */
    XFlush(display);
}

void x11_cleanup() {
    /* Cleans everything up, be sure to call when done */
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

POS get_mouse_pos() {
    /* Returns true if the left mouse button is pressed */
    Window child; // child window the pointer is in, if any
    Window root; // root window the pointer is in
    int root_x, root_y; // pointer position relative to the root window
    int win_x, win_y; // pointer position relative to the window
    unsigned int button_state; // state of the modifier keys and the buttons as a bitmask
    
    // Query the pointer state
    XQueryPointer(display, window, &root, &child, &root_x, &root_y, &win_x, &win_y, &button_state);
    
    // Return the position
    POS pos = {win_x, win_y};
    return pos;
}

bool is_lmb_pressed() {
    /* Returns true if the left mouse button is pressed */
    Window child; // child window the pointer is in, if any
    Window root; // root window the pointer is in
    int root_x, root_y; // pointer position relative to the root window
    int win_x, win_y; // pointer position relative to the window
    unsigned int button_state; // state of the modifier keys and the buttons as a bitmask
    
    // Query the pointer state
    XQueryPointer(display, window, &root, &child, &root_x, &root_y, &win_x, &win_y, &button_state);
    
    // Check if lmb is being pressed
    if (button_state & Button1Mask) {
        return true;
    }
    return false;
}

bool check_for_keybind(char* key) {
    KeyCode keycode = XKeysymToKeycode(display, XStringToKeysym(key));
    /* Checks for the keybind and returns true if one is found */
    XEvent event;
    if (XCheckMaskEvent(display, KeyPressMask, &event)) {
        if (event.xkey.keycode == keycode) {
            return true;
        } else {
            XPutBackEvent(display, &event);
        }
    }
    return false;
}

bool wait_for_keybind(char* key) {
    /* Waits for the keybind and returns true if one is found */
    KeyCode keycode = XKeysymToKeycode(display, XStringToKeysym(key));
    XEvent event;
    while (true) {
        XNextEvent(display, &event);
        if (event.xkey.keycode == keycode && event.type == KeyPress) {
            return true;
        }
    }
}

void setup_keybind(char* key) {
    /* Sets up the keybind for the window */
    KeyCode keycode = XKeysymToKeycode(display, XStringToKeysym(key));
    XGrabKey(display, keycode, ControlMask | Mod1Mask, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
}

Window* get_window() {
    /* Returns the window */
    return &window;
}

Display* window_setup(ARGB bg_color) {
    /* Main helper function to run here. Will do all the window setup
    Returns up a pointer to the display if you want it */
    display = XOpenDisplay(NULL);
    screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    // Find an ARGB visual for transparency
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display, screen, 32, TrueColor, &vinfo)) {
        fprintf(stderr, "No ARGB visual found.\n");
        return NULL;
    }

    // Create a colormap
    Colormap colormap = XCreateColormap(display, root, vinfo.visual, AllocNone);

    // Set window attributes
    XSetWindowAttributes attrs;
    attrs.colormap = colormap;
    attrs.background_pixel = argb_to_int(bg_color);
    attrs.border_pixel = 0;

    // Create the window
    window = XCreateWindow(display, root, 0, 0, screen_width(), screen_height(), 0,
                                  vinfo.depth, InputOutput, vinfo.visual,
                                  CWColormap | CWBackPixel | CWBorderPixel, &attrs);

    // Set the window type to desktop
    Atom window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom desktop_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    
    XChangeProperty(display, window, window_type, XA_ATOM, 32, PropModeReplace, (uchar*) &desktop_type, 1);

    // Show the window
    XMapWindow(display, window);

    // Initialize the graphics context
    gc = XCreateGC(display, window, 0, NULL);

    // Lower the window below everything and disable input
    lower_window();

    // Listen for certain events
    XSelectInput(display, window, KeyPressMask | ButtonPressMask);

    // Setup the mouse input for the window
    XGrabPointer(display, window, True, 
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

    return display;
}

#endif // X11_LIB