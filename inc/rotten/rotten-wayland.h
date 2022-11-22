/************************************************************************************************************
 * @file Rotten window wayland specific header
 * @brief Provides a method for dynamically loading wayland function pointers at runtime and the handles to
 * extract the native wayland handles for a rotten wayland window to allow users more fine grained control
 * over the window when they need it.
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 ************************************************************************************************************/
#ifndef __ROTTEN_EXTERNAL_WINDOW_WAYLAND_H__
#define __ROTTEN_EXTERNAL_WINDOW_WAYLAND_H__ (1)
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include "rotten-core.h"
#include "rotten-dynamic-loading.h"
ROTTEN_CPP_GUARD

typedef struct rotten_library_wayland {
    // Native linux handle for libwayland-client.so
    rotten_dynamic_library* way_lib;

    //
    // Function pointer table
    //

    struct wl_display* (*display_connect)(const char*);
    void (*display_disconnect)(struct wl_display*);
    struct wl_surface* (*compositor_create_surface)(struct wl_compositor* compositor);

} rotten_library_wayland;

typedef struct rotten_window_wayland_extra {
    struct wl_display* display;  // Pointer to the information about the current display
} rotten_window_wayland_extra;

rotten_success_code rotten_library_wayland_load_min(rotten_library_wayland* lib);

rotten_success_code rotten_library_wayland_valid_session(rotten_library_wayland* lib);

rotten_success_code rotten_library_wayland_load_full(rotten_library_wayland* lib);

rotten_success_code rotten_library_wayland_close(rotten_library_wayland* lib);
ROTTEN_CPP_GUARD_END
#endif