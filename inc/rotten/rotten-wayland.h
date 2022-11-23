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

    // Pointer to a const structs which are exported const symbols from the wayland library, we instead fetch
    // pointers to them. Using the same method as function pointers
    struct wl_interface* registry_interface;
    struct wl_interface* compositor_interface;

} rotten_library_wayland;

typedef struct rotten_window_wayland_extra {
    struct wl_display* display;  // Pointer to the information about the current display
} rotten_window_wayland_extra;

rotten_success_code rotten_library_wayland_load_min(rotten_library_wayland* lib);

rotten_success_code rotten_library_wayland_valid_session(rotten_library_wayland* lib);

rotten_success_code rotten_library_wayland_load_full(rotten_library_wayland* lib);

rotten_success_code rotten_library_wayland_close(rotten_library_wayland* lib);

//
// Wayland has quite a few static inline functions which use the default symbols. obviously we cant use them
// as they will through a linking error, so provide the most scuffed wrapper ever. They will be
// reimplementations of the static inlines, just taking an extra rotten_library_wayland pointer
// parameter
//

struct wl_registry* rotten_wl_display_get_registry(rotten_library_wayland* lib, struct wl_display* display);

ROTTEN_CPP_GUARD_END
#endif