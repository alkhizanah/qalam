#include <assert.h>
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
#include <xkbcommon/xkbcommon.h>

#include "wayland-protocols/xdg-shell-client-protocol.h"

#include "app.h"
#include "platform.h"

unsigned int platform_horizontal_resolution = 96;
unsigned int platform_vertical_resolution = 96;

static App app;

static bool window_should_close = false;

static struct wl_display *wl_display;
static struct wl_registry *wl_registry;
static struct wl_surface *wl_surface;
static struct wl_shm *wl_shm;
static struct wl_compositor *wl_compositor;
static struct wl_seat *wl_seat;
static struct wl_keyboard *wl_keyboard;
static struct xdg_wm_base *xdg_wm_base;
static struct xdg_surface *xdg_surface;
static struct xdg_toplevel *xdg_toplevel;
static struct xkb_context *xkb_context;
static struct xkb_keymap *xkb_keymap;
static struct xkb_state *xkb_state;

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
    int stride = app.framebuffer.width * 4;

    int size = stride * app.framebuffer.height;

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
        pool, 0, app.framebuffer.width, app.framebuffer.height, stride,
        WL_SHM_FORMAT_XRGB8888);

    wl_shm_pool_destroy(pool);

    close(fd);

    app_update(&app);

    for (size_t i = 0; i < app.framebuffer.width * app.framebuffer.height;
         i++) {
        const Color color = app.framebuffer.pixels[i];

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

static void wl_keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard,
                               uint32_t format, int32_t fd, uint32_t size) {
    (void)data;
    (void)wl_keyboard;

    assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

    char *map_shm = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);

    assert(map_shm != MAP_FAILED);

    xkb_keymap_unref(xkb_keymap);
    xkb_state_unref(xkb_state);

    xkb_keymap = xkb_keymap_new_from_string(xkb_context, map_shm,
                                            XKB_KEYMAP_FORMAT_TEXT_V1,
                                            XKB_KEYMAP_COMPILE_NO_FLAGS);

    munmap(map_shm, size);

    close(fd);

    xkb_state = xkb_state_new(xkb_keymap);
}

static void wl_keyboard_enter(void *data, struct wl_keyboard *wl_keyboard,
                              uint32_t serial, struct wl_surface *surface,
                              struct wl_array *keys) {
    (void)data;
    (void)wl_keyboard;
    (void)surface;
    (void)serial;
    (void)keys;
}

static void wl_keyboard_leave(void *data, struct wl_keyboard *wl_keyboard,
                              uint32_t serial, struct wl_surface *surface) {
    (void)data;
    (void)wl_keyboard;
    (void)surface;
    (void)serial;
}

static void wl_keyboard_modifiers(void *data, struct wl_keyboard *wl_keyboard,
                                  uint32_t serial, uint32_t mods_depressed,
                                  uint32_t mods_latched, uint32_t mods_locked,
                                  uint32_t group) {
    (void)data;
    (void)wl_keyboard;
    (void)serial;

    xkb_state_update_mask(xkb_state, mods_depressed, mods_latched, mods_locked,
                          0, 0, group);
}

static void wl_keyboard_repeat_info(void *data, struct wl_keyboard *wl_keyboard,
                                    int32_t rate, int32_t delay) {
    (void)data;
    (void)wl_keyboard;
    (void)rate;
    (void)delay;
}

static void wl_keyboard_key(void *data, struct wl_keyboard *wl_keyboard,
                            uint32_t serial, uint32_t time, uint32_t key,
                            uint32_t state) {
    (void)data;
    (void)wl_keyboard;
    (void)serial;
    (void)time;
    (void)key;
    (void)state;
}

static const struct wl_keyboard_listener wl_keyboard_listener = {
    .keymap = wl_keyboard_keymap,
    .enter = wl_keyboard_enter,
    .leave = wl_keyboard_leave,
    .key = wl_keyboard_key,
    .modifiers = wl_keyboard_modifiers,
    .repeat_info = wl_keyboard_repeat_info,
};

static void wl_seat_capabilities(void *data, struct wl_seat *wl_seat,
                                 uint32_t capabilities) {
    (void)data;
    (void)wl_seat;

    bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

    if (have_keyboard && wl_keyboard == NULL) {
        wl_keyboard = wl_seat_get_keyboard(wl_seat);
        wl_keyboard_add_listener(wl_keyboard, &wl_keyboard_listener, NULL);
    } else if (!have_keyboard && wl_keyboard != NULL) {
        wl_keyboard_release(wl_keyboard);
        wl_keyboard = NULL;
    }
}

static void wl_seat_name(void *data, struct wl_seat *wl_seat,
                         const char *name) {
    (void)data;
    (void)wl_seat;
    (void)name;
}

static const struct wl_seat_listener wl_seat_listener = {
    .capabilities = wl_seat_capabilities,
    .name = wl_seat_name,
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
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        wl_seat = wl_registry_bind(wl_registry, name, &wl_seat_interface, 5);

        wl_seat_add_listener(wl_seat, &wl_seat_listener, NULL);
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

    xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

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

    app.theme = kanagawa_wave();

    app.file_path = "some_file.c";

    app.file_content = "#include <stdio.h>\n"
                       "\n"
                       "int add(int a, int b) {\n"
                       "    return a + b;\n"
                       "}\n"
                       "\n"
                       "int main(void) {\n"
                       "    int x = 2;\n"
                       "    int y = 3;\n"
                       "\n"
                       "    printf(\"sum = %d\\n\", add(x, y));\n"
                       "    return 0;\n"
                       "}\n";

    const char *font_family = "JetBrains Mono NerdFont";

    if (!platform_set_font(font_family)) {
        fprintf(stderr, "error: could not set font to %s\n", font_family);

        return 1;
    }

    if (!platform_set_font_size(11)) {
        fprintf(stderr, "error: could not set font size\n");

        return 1;
    }

    while (wl_display_dispatch(wl_display) && window_should_close != true)
        ;

    return 0;
}
