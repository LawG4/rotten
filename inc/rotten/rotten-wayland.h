/************************************************************************************************************
 * @file Rotten window wayland specific header
 * @brief Provides a method for dynamically loading wayland function pointers at runtime and the handles to
 * extract the native wayland handles for a rotten wayland window to allow users more fine grained control
 * over the window when they need it.
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 *
 * @note From what I can tell, xdg_wm_base requires the programmer to use wayland-scanner to generate source
 * and header files for this interface. libwayland-client.so does not have any xdg symbols. I don't want to
 * use those sources inside rotten window as it would force a direct link to wayland. until then we cheat with
 * wayland-egl.so
 ************************************************************************************************************/
#ifndef __ROTTEN_EXTERNAL_WINDOW_WAYLAND_H__
#define __ROTTEN_EXTERNAL_WINDOW_WAYLAND_H__ (1)
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include "rotten-core.h"
#include "rotten-dynamic-loading.h"
#include "rotten-window.h"
ROTTEN_CPP_GUARD

// The library struct is pretty massive, forward declare it and define it the bottom of the file
typedef struct rotten_library_wayland rotten_library_wayland;
struct xdg_toplevel;
struct xdg_wm_base;

typedef struct rotten_window_wayland_extra {
    struct wl_display* display;        // Pointer to the information about the current display
    struct wl_registry* registry;      // Proxy handle to the global resource manager
    struct wl_compositor* compositor;  // Proxy handle to the global compositor
    struct wl_shell* shell;            // Proxy handle to the shell system
    struct xdm_wm_base* wm_base;       // Proxy handle to the window manager roles

    struct wl_surface* surface;         // A rectangle which we can display contents to
    struct xdg_surface* xdg_surface;    // A derivative object for the surface to comunicate with wm
    struct xdg_toplevel* xdg_toplevel;  // wm top level roll
} rotten_window_wayland_extra;

typedef struct rotten_window_wayland {
    rotten_window_base base;             // Default information
    rotten_window_wayland_extra extra;   // Wayland specific information for the window
    struct rotten_library_wayland* way;  // handle to the wayland library functions
} rotten_window_wayland;

//
// Library runtime loaders
//

rotten_success_code rotten_library_wayland_load_min(rotten_library_wayland* lib);

rotten_success_code rotten_library_wayland_valid_session(rotten_library_wayland* lib);

rotten_success_code rotten_library_wayland_load_full(rotten_library_wayland* lib);

rotten_success_code rotten_library_wayland_close(rotten_library_wayland* lib);

//
// Helper functions for working within rotten wayland
// For example a static ping ponger or interface listener etc...
//

/**
 * @brief Attaches the internal static functon to the rotten window's registry and ensures that a pointer to
 * the window is passed to this function pointer, and this will ensure that the proxies for interfaces can be
 * given to a rotten window
 * @param window Pointer to the rotten wayland window
 * @returns success code.
 */
rotten_success_code rotten_wl_attach_interface_listeners(rotten_window_wayland* window);

//
// Wayland has quite a few static inline functions which use the default symbols. obviously we cant use them
// as they will through a linking error, so provide the most scuffed wrapper ever. They will be
// reimplementations of the static inlines, just taking an extra rotten_library_wayland pointer
// parameter
//

struct wl_registry* rotten_wl_display_get_registry(rotten_library_wayland* lib, struct wl_display* display);

int rotten_wl_registry_add_listener(rotten_library_wayland* lib, struct wl_registry* registry,
                                    const struct wl_registry_listener* listener, void* data);

void* rotten_wl_registry_bind(rotten_library_wayland* lib, struct wl_registry* registry, uint32_t id,
                              const struct wl_interface* interface, uint32_t version);

struct wl_surface* rotten_wl_compositor_create_surface(rotten_library_wayland* lib,
                                                       struct wl_compositor* compositor);

void rotten_wl_surface_commit(rotten_library_wayland* lib, struct wl_surface* wl_surface);

typedef struct rotten_library_wayland {
    // Native linux handle for libwayland-client.so
    rotten_dynamic_library* way_lib;

    // Native linux handle for lib librotten-wayland-xdg.so
    rotten_dynamic_library* xdg_lib;

    //
    // Function pointer table
    //

    // Display functions
    struct wl_display* (*display_connect)(const char*);
    void (*display_disconnect)(struct wl_display*);
    int (*display_dispatch)(struct wl_display* display);
    int (*display_roundtrip)(struct wl_display* display);
    struct wl_surface* (*compositor_create_surface)(struct wl_compositor* compositor);
    // Proxy functions
    uint32_t (*proxy_get_version)(struct wl_proxy* prozy);
    struct wl_proxy* (*proxy_marshal_flags)(struct wl_proxy*, uint32_t opcode,
                                            const struct wl_interface* interface, uint32_t version,
                                            uint32_t flags, ...);
    int (*proxy_add_listener)(struct wl_proxy*, void (**implementation)(void), void* data);

    // xdg function pointers
    struct xdg_surface* (*xdg_wm_base_get_xdg_surface)(struct rotten_library_wayland* way,
                                                       struct xdg_wm_base* xdg_wm_base,
                                                       struct wl_surface* surface);
    struct xdg_toplevel* (*xdg_surface_get_toplevel)(struct rotten_library_wayland* way,
                                                     struct xdg_surface* surface);
    void (*xdg_toplevel_set_title)(struct rotten_library_wayland* way, struct xdg_toplevel* xdg_toplevel,
                                   const char* title);

    // Pointer to a const structs which are exported const symbols from the wayland library, we instead fetch
    // pointers to them. Using the same method as function pointers
    struct wl_interface* registry_interface;
    struct wl_interface* compositor_interface;
    struct wl_interface* surface_interface;
    const struct wl_interface* xdg_wm_base_interface;
    const struct wl_interface* xdg_surface_interface;
    const struct wl_interface* xdg_toplevel_interface;

} rotten_library_wayland;

ROTTEN_CPP_GUARD_END
#endif