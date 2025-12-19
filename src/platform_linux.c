#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "loop.h"

int main() {
    Display *display = XOpenDisplay(NULL);

    if (display == NULL) {
        fprintf(stderr, "error: failed to open x11 display\n");

        return 1;
    }

    Window root = DefaultRootWindow(display);

    if (root == None) {
        fprintf(stderr, "error: no x11 root window found");

        XCloseDisplay(display);

        return 1;
    }

    Window window =
        XCreateSimpleWindow(display, root, 0, 0, 800, 600, 0, 0, 0xffffffff);

    if (window == None) {
        fprintf(stderr, "error: failed to create x11 window");

        XCloseDisplay(display);

        return 1;
    }

    XSelectInput(display, window, StructureNotifyMask);

    XMapWindow(display, window);

    XWindowAttributes wa;

    XGetWindowAttributes(display, window, &wa);

    Loop loop = loop_init(wa.width, wa.height);

    uint32_t *backbuffer = malloc(loop.width * loop.height * sizeof(uint32_t));

    XImage *image = XCreateImage(display, wa.visual, wa.depth, ZPixmap, 0,
                                 (char *)backbuffer, loop.width, loop.height,
                                 sizeof(Color) * 8, loop.width * sizeof(Color));

    GC gc = XCreateGC(display, window, 0, NULL);

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);

    XSetWMProtocols(display, window, &wm_delete_window, 1);

    XEvent event;

    while (true) {
        while (XPending(display)) {
            XNextEvent(display, &event);

            switch (event.type) {
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
                    XDestroyWindow(display, window);
                    XCloseDisplay(display);

                    return 0;
                }

                break;
            case ConfigureNotify:
                if (event.xconfigure.width != (int)loop.width ||
                    event.xconfigure.height != (int)loop.height) {
                    loop_resize(&loop, event.xconfigure.width,
                                event.xconfigure.height);

                    backbuffer = realloc(backbuffer, loop.width * loop.height *
                                                         sizeof(uint32_t));

                    XFree(image);

                    image = XCreateImage(display, wa.visual, wa.depth, ZPixmap,
                                         0, (char *)backbuffer, loop.width,
                                         loop.height, sizeof(Color) * 8,
                                         loop.width * sizeof(Color));
                }

                break;
            }
        }

        loop_update(&loop);

        for (size_t i = 0; i < loop.width * loop.height; i++) {
            const Color color = loop.framebuffer[i];

            backbuffer[i] = color.r << 16 | color.g << 8 | color.b;
        }

        XPutImage(display, window, gc, image, 0, 0, 0, 0, loop.width,
                  loop.height);
    }
}
