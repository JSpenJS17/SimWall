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
void fill_cell(int x, int y, int size) {
    /* Fills a cell at x, y with the current color */
    XFillRectangle(display, window, gc, x*size, y*size, size, size);
}

void fill_circle(int x, int y, int size) {
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

int rgb_to_int(RGB rgb) {
    /* Converts an RGB struct to an int for X11 compatibility */
    return rgb.r << 16 | rgb.g << 8 | rgb.b;
}

void color(RGB rgb) {
    /* Sets foreground paint color using RGB struct */
    XSetForeground(display, gc, rgb_to_int(rgb));
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

static void disable_input(Window in_win) {
    /* Disables the input region of a specific window */
    Region region = XCreateRegion();
    XShapeCombineRegion(display, in_win, ShapeInput, 0, 0, region, ShapeSet);
    XDestroyRegion(region);
}

void x11_cleanup() {
    /* Cleans everything up, be sure to call when done */
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void display_image(char* path, int x, int y) {
    /* Displays an image at x, y */
    // Open the image
    FILE* image = fopen(path, "r");
    if (image == NULL) {
        fprintf(stderr, "Error opening image file\n");
        return;
    }

    // Read the image
    fseek(image, 0, SEEK_END);
    long size = ftell(image);
    fseek(image, 0, SEEK_SET);
    uchar* data = malloc(size);
    fread(data, 1, size, image);
    fclose(image);

    // Create the pixmap
    Pixmap pixmap = XCreatePixmap(display, window, 100, 100, DefaultDepth(display, screen));
    XImage* ximage = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, data, 100, 100, 32, 0);
    XPutImage(display, pixmap, gc, ximage, 0, 0, 0, 0, 100, 100);

    // Draw the image
    XCopyArea(display, pixmap, window, gc, 0, 0, 100, 100, x, y);

    // Free the data
    free(data);
    XFreePixmap(display, pixmap);
    XDestroyImage(ximage);
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

Display* window_setup(RGB bg_color) {
    /* Main helper function to run here. Will do all the window setup
    Returns up a pointer to the display if you want it */
    display = XOpenDisplay(NULL);
    screen = DefaultScreen(display);
    Window root = DefaultRootWindow(display);

    // Create the window
    window = XCreateSimpleWindow(display, root, 0, 0, 
                                 screen_width(), screen_height(), 
                                 1, rgb_to_int(bg_color), rgb_to_int(bg_color));

    // Set the window type to desktop
    Atom window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom desktop_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    
    XChangeProperty(display, window, window_type, XA_ATOM, 32, PropModeReplace, (uchar*) &desktop_type, 1);

    // Show the window
    XMapWindow(display, window);

    // Initialize the graphics context
    gc = XCreateGC(display, window, 0, 0);

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