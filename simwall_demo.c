#include "life_lib.c"

int main(int argc, char **argv) {
    // parse arguments
    unsigned char flags = parse_args(argc, argv);

    // Initialize the window
    window_setup(flags);

    // Set up event handler
    XEvent event;
    
    // Event loop
    while (1) {
        XNextEvent(display, &event);

        // might have to refactor this - probably never see this event
        if (event.type == Expose) {

            // Set the foreground color (black)
            XSetForeground(display, gc, BlackPixel(display, screen));
            // When the window is exposed (shown), fill a black background
            XFillRectangle(display, window.window, gc, 0, 0, window.width, window.height);
            // Set the color to white to draw a square
            XSetForeground(display, gc, WhitePixel(display, screen));
            // Draw in a SINGLE cell in the middle of the screen
            XFillRectangle(display, window.window, gc,                               // (magic)
                            window.width/2 - CELL_SIZE, window.height/2 - CELL_SIZE, // (x, y)
                            CELL_SIZE, CELL_SIZE);                                   // (width, height)

        }
    }

    // Cleanup
    XFreeGC(display, gc);
    XDestroyWindow(display, window.window);
    XCloseDisplay(display);

    return 0;
}
