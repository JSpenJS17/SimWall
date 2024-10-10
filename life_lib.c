/* life_lib.c 
Library file for the game of life displaying to X11
Will basically have some X11 helpers that will be used in the main file
*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xproto.h>

#include <X11/extensions/shape.h>
#include <X11/extensions/Xrender.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

// Helpful macros and globals
#define OPAQUE 0xffffffff

#define NAME "SimWall"
#define VERSION "0.1"
#define CELL_SIZE 25

#define ATOM(a) XInternAtom(display, #a, False)

// Flag setup for later if there are more args
#define DAEMONIZE 0b00000001

// Globals for display information
Display *display = NULL;
int display_width;
int display_height;
int screen;

// Graphics context for drawing
GC gc;

// Window struct to hold all of our window needs
struct window {
  Window root, window, desktop;
  Drawable drawable;
  Visual *visual;
  Colormap colourmap;

  unsigned int width;
  unsigned int height;
  int x;
  int y;
} window;

static pid_t pid = 0;

static char **childArgv = 0;
static int nChildArgv = 0;

static void init_x11() {
    /* inits an x11 display, called by window_setup() */
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, NAME ": Error: couldn't open display\n");
        return;
    }
    screen = DefaultScreen(display);
    display_width = DisplayWidth(display, screen);
    display_height = DisplayHeight(display, screen);
}

static Window find_subwindow(Window win, int w, int h) {
    /* Finds a subwindow. Called by find_desktop_window() */
    unsigned int i, j;
    Window troot, parent, *children;
    unsigned int n;

    /* search subwindows with same size as display or work area */

    for (i = 0; i < 10; i++) {
        XQueryTree(display, win, &troot, &parent, &children, &n);

        for (j = 0; j < n; j++) {
            XWindowAttributes attrs;

            if (XGetWindowAttributes(display, children[j], &attrs)) {
                /* Window must be mapped and same size as display or
                * work space */
                if (attrs.map_state != 0 &&
                    ((attrs.width == display_width && attrs.height == display_height) ||
                    (attrs.width == w && attrs.height == h))) {
                    win = children[j];
                    break;
                }
            }
        }

        XFree(children);
        if (j == n) {
            break;
        }
    }

    return win;
}

static Window find_desktop_window(Window *p_root, Window *p_desktop) {
    /* Finds the user's desktop window. Called by window_setup() */
    Atom type;
    int format, i;
    unsigned long nitems, bytes;
    unsigned int n;
    Window root = RootWindow(display, screen);
    Window win = root;
    Window troot, parent, *children;
    unsigned char *buf = NULL;

    if (!p_root || !p_desktop) {
        return 0;
    }

    /* some window managers set __SWM_VROOT to some child of root window */

    XQueryTree(display, root, &troot, &parent, &children, &n);
    for (i = 0; i < (int)n; i++) {
        if (XGetWindowProperty(display, children[i], ATOM(__SWM_VROOT), 0, 1, False,
                            XA_WINDOW, &type, &format, &nitems, &bytes,
                            &buf) == Success &&
            type == XA_WINDOW) {
            win = *(Window *)buf;
            XFree(buf);
            XFree(children);
            fflush(stderr);
            *p_root = win;
            *p_desktop = win;
            return win;
        }

        if (buf) {
            XFree(buf);
            buf = 0;
        }
    }
    XFree(children);

    /* get subwindows from root */
    win = find_subwindow(root, -1, -1);

    display_width = DisplayWidth(display, screen);
    display_height = DisplayHeight(display, screen);

    win = find_subwindow(win, display_width, display_height);

    if (buf) {
        XFree(buf);
        buf = 0;
    }

    fflush(stderr);

    *p_root = root;
    *p_desktop = win;

    return win;
}

unsigned char parse_args(int argc, char **argv) {
    /* Parses the args for the program. 
    Returns a flag byte with some flags! */
    unsigned char flags = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-D") == 0) {
            flags |= DAEMONIZE;
        }
    }

    return flags;
}

int window_setup(unsigned char args) {
    /* Main helper function to run here. Will do all the window setup 
    Takes flag byte parsed by parse_args */

    /* ALL OF THIS TO SET UP THE WINDOW! */

    // screen #
    int screen;

    // start up x11 libs, standard stuff
    init_x11();
    if (!display) {
        return 1;
    }

    // define the depth we want (0) and some flags
    int depth = 0, flags = CWOverrideRedirect | CWBackingStore;
    // no clue what this is
    Visual *visual = NULL;

    // FIND our DESKTOP WINDOW! very important
    if (!find_desktop_window(&window.root, &window.desktop)) {
        fprintf(stderr, NAME ": Error: couldn't find desktop window\n");
        return 1;
    }

    // Set some attrs, I have no idea what any of these do :D
    XSetWindowAttributes attrs = {ParentRelative,
                                  0L,
                                  0,
                                  0L,
                                  0,
                                  0,
                                  Always,
                                  0L,
                                  0L,
                                  False,
                                  StructureNotifyMask | ExposureMask |
                                      ButtonPressMask | ButtonReleaseMask,
                                  0L,
                                  False,
                                  0,
                                  0};

    // hints, again not sure. X11 magic, probably. WM = window manager
    XWMHints wmHint;

    // Set some variables in the window struct for reference later
    window.x = 0;
    window.y = 0;
    window.width = DisplayWidth(display, screen);
    window.height = DisplayHeight(display, screen);

    // Create the WINDOW!
    window.window = XCreateWindow(display, window.root, window.x, window.y,
                                  window.width, window.height, 0, depth,
                                  InputOutput, visual, flags, &attrs);

    // set some window manager hint flags
    wmHint.flags = InputHint | StateHint;
    wmHint.initial_state = NormalState;

    // Set the properties with the hints.
        // Used to take in argv and argc, but we don't need this in our specific use case
                                                       /*argv  argc*/
    XSetWMProperties(display, window.window, NULL, NULL, NULL, 0   , NULL, &wmHint, NULL);

    // create some Atom objects I guess, I dunno what they do
    Atom xa = ATOM(_NET_WM_WINDOW_TYPE);
    Atom prop = ATOM(_NET_WM_WINDOW_TYPE_DESKTOP);

    // Absolute magic
    XChangeProperty(display, window.window, xa, XA_ATOM, 32, PropModeReplace, (unsigned char *)&prop, 1);

    // Select input events to listen for
    XSelectInput(display, window.window, ExposureMask | KeyPressMask);

    // No input (click on desktop, you see icons) code
        // looks like we make a touchable region, then kill it so we can't touch it anymore
    Region region;

    region = XCreateRegion();
    if (region) {
        XShapeCombineRegion(display, window.window, ShapeInput, 0, 0, region, ShapeSet);
        XDestroyRegion(region);
    }

    /* DAEMON STUFF REAL QUICK */
    if (args & DAEMONIZE) {
        pid = fork();
        if (pid != 0) {
            // end parent early to effectively start as a daemon
            return 0;
        }
    }

    // Make the window visible!!
    XMapWindow(display, window.window);


    // Create a graphics context for drawing
    gc = XCreateGC(display, window.window, 0, NULL);

    /* WINDOW FINALLY SET UP */
}