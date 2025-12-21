#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include <wayland-client.h>

#include "wayland-protocols/xdg-shell-client-protocol.h"

#include "app.h"

static App app;

static bool window_should_close = false;

static struct wl_display *wl_display;
static struct wl_registry *wl_registry;
static struct wl_surface *wl_surface;
static struct wl_shm *wl_shm;
static struct wl_compositor *wl_compositor;
static struct xdg_wm_base *xdg_wm_base;
static struct xdg_surface *xdg_surface;
static struct xdg_toplevel *xdg_toplevel;

static void randname(char *buf) {
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);

    long r = ts.tv_nsec;

    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A' + (r & 15) + (r & 16) * 2;

        r >>= 5;
    }
}

static int create_shm_file(void) {
    int retries = 100;

    do {
        char name[] = "/wl_shm-XXXXXX";

        randname(name + sizeof(name) - 7);

        --retries;

        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);

        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);

    return -1;
}

static int allocate_shm_file(size_t size) {
    int fd = create_shm_file();

    if (fd < 0) {
        return -1;
    }

    int ret;

    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

static void wl_buffer_release(void *data, struct wl_buffer *wl_buffer) {
    (void)data;

    wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release,
};

static struct wl_buffer *platform_copy_frame(void) {
    int stride = app.width * 4;

    int size = stride * app.height;

    int fd = allocate_shm_file(size);

    if (fd == -1) {
        return NULL;
    }

    uint32_t *backbuffer =
        mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (backbuffer == MAP_FAILED) {
        close(fd);

        return NULL;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(wl_shm, fd, size);

    struct wl_buffer *buffer = wl_shm_pool_create_buffer(
        pool, 0, app.width, app.height, stride, WL_SHM_FORMAT_XRGB8888);

    wl_shm_pool_destroy(pool);

    close(fd);

    app_update(&app);

    for (size_t i = 0; i < app.width * app.height; i++) {
        const Color color = app.framebuffer[i];

        backbuffer[i] = color.r << 16 | color.g << 8 | color.b;
    }

    munmap(backbuffer, size);

    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);

    return buffer;
}

static const struct wl_callback_listener wl_surface_frame_listener;

static void wl_surface_frame_done(void *data, struct wl_callback *cb,
                                  uint32_t time) {
    (void)data;
    (void)time;

    wl_callback_destroy(cb);

    cb = wl_surface_frame(wl_surface);
    wl_callback_add_listener(cb, &wl_surface_frame_listener, NULL);

    struct wl_buffer *buffer = platform_copy_frame();

    wl_surface_attach(wl_surface, buffer, 0, 0);
    wl_surface_damage_buffer(wl_surface, 0, 0, INT32_MAX, INT32_MAX);
    wl_surface_commit(wl_surface);
}

static const struct wl_callback_listener wl_surface_frame_listener = {
    .done = wl_surface_frame_done,
};

static void xdg_toplevel_configure(void *data, struct xdg_toplevel *toplevel,
                                   int32_t width, int32_t height,
                                   struct wl_array *states) {
    (void)data;
    (void)toplevel;
    (void)states;

    if (width == 0 || height == 0) {
        return;
    }

    app_resize(&app, width, height);
}

static void xdg_toplevel_close(void *data, struct xdg_toplevel *toplevel) {
    (void)data;
    (void)toplevel;

    window_should_close = true;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure,
    .close = xdg_toplevel_close,
};

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
                                  uint32_t serial) {
    (void)data;

    xdg_surface_ack_configure(xdg_surface, serial);

    struct wl_buffer *buffer = platform_copy_frame();

    wl_surface_attach(wl_surface, buffer, 0, 0);
    wl_surface_commit(wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base,
                             uint32_t serial) {
    (void)data;

    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static void registry_global(void *data, struct wl_registry *wl_registry,
                            uint32_t name, const char *interface,
                            uint32_t version) {
    (void)data;
    (void)version;

    if (strcmp(interface, wl_shm_interface.name) == 0) {
        wl_shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
        wl_compositor =
            wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        xdg_wm_base =
            wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(xdg_wm_base, &xdg_wm_base_listener, NULL);
    }
}

static void registry_global_remove(void *data, struct wl_registry *wl_registry,
                                   uint32_t name) {
    (void)data;
    (void)wl_registry;
    (void)name;
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

int platform_main_loop(void) {
    wl_display = wl_display_connect(NULL);

    wl_registry = wl_display_get_registry(wl_display);

    wl_registry_add_listener(wl_registry, &wl_registry_listener, NULL);

    wl_display_roundtrip(wl_display);

    wl_surface = wl_compositor_create_surface(wl_compositor);

    xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, wl_surface);

    xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, NULL);

    xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);

    xdg_toplevel_add_listener(xdg_toplevel, &xdg_toplevel_listener, NULL);

    xdg_toplevel_set_title(xdg_toplevel, "Qalam");

    wl_surface_commit(wl_surface);

    struct wl_callback *cb = wl_surface_frame(wl_surface);

    wl_callback_add_listener(cb, &wl_surface_frame_listener, NULL);

    app_init(0, 0);

    while (wl_display_dispatch(wl_display) && window_should_close != true)
        ;

    return 0;
}
