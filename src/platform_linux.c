#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "app.h"

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

    App app = app_init(wa.width, wa.height);

    uint32_t *backbuffer = malloc(app.width * app.height * sizeof(uint32_t));

    XImage *image = XCreateImage(display, wa.visual, wa.depth, ZPixmap, 0,
                                 (char *)backbuffer, app.width, app.height,
                                 sizeof(Color) * 8, app.width * sizeof(Color));

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
                if (event.xconfigure.width != (int)app.width ||
                    event.xconfigure.height != (int)app.height) {
                    app_resize(&app, event.xconfigure.width,
                                event.xconfigure.height);

                    backbuffer = realloc(backbuffer, app.width * app.height *
                                                         sizeof(uint32_t));

                    XFree(image);

                    image = XCreateImage(display, wa.visual, wa.depth, ZPixmap,
                                         0, (char *)backbuffer, app.width,
                                         app.height, sizeof(Color) * 8,
                                         app.width * sizeof(Color));
                }

                break;
            }
        }

        app_update(&app);

        for (size_t i = 0; i < app.width * app.height; i++) {
            const Color color = app.framebuffer[i];

            backbuffer[i] = color.r << 16 | color.g << 8 | color.b;
        }

        XPutImage(display, window, gc, image, 0, 0, 0, 0, app.width,
                  app.height);
    }
}
