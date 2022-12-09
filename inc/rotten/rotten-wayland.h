/************************************************************************************************************
 * @file Rotten window wayland specific header
 * @brief This library does not link to wayland, instead it provides a method for dynamically loading wayland
 * function pointers at runtime. This allows users to querey for wayland support themselves and then fallback
 * to another system if not present.
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 *
 * @note A lot of wayland is implemented inside the protocol extensions, the symbols for these extensions are
 * not located inside the libwayland-client library, so we cannot retrieve those symbols at runtime. Instead
 * users have to build these from source which directly links to wayland, what we do is compile those
 * extensions into a shared library which can be loaded at runtime as well. See rotten-wayland-ext
 ************************************************************************************************************/
#ifndef __ROTTEN_EXTERNAL_WINDOW_WAYLAND_H__
#define __ROTTEN_EXTERNAL_WINDOW_WAYLAND_H__ (1)
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include "rotten-core.h"
#include "rotten-dynamic-loading.h"
#include "rotten-window.h"

// Library not linked to, just fetch definitions
#include "rotten-wayland-ext.h"
ROTTEN_CPP_GUARD

/**
 * @brief Dispatch table for all of the function pointers we need to load from libwayland-client at runtime
 * @note This library is pretty big, so find the definition at the bottom of rotten-wayland.h
 * When the user is using the rotten-wayland-ext library they only need to fetch one function pointer to fill
 * this struct
 */
typedef struct rotten_library_wayland rotten_library_wayland;

/**
 * @brief All of the state used by rotten window for the core wayland protocol
 */
typedef struct rotten_window_wayland_core_state {
    struct wl_display* display;        // Pointer to the information about the current display
    struct wl_registry* registry;      // Proxy handle to the global resource manager
    struct wl_compositor* compositor;  // Proxy handle to the global compositor
    struct wl_shm* shared_mem;         //
    struct wl_shell* shell;            // Proxy handle to the shell system
} rotten_window_wayland_core_state;

typedef struct rotten_window_wayland {
    rotten_window_base base;                      // Default information for rotten window
    rotten_window_wayland_core_state core_state;  // Wayland core protocol state
    rotten_window_wayland_ext_state ext_state;    // Protocol extensions state
    struct rotten_library_wayland* way;           // handle to the wayland library functions
    struct rotten_library_wayland_ext* ext;       // handle to the protocol extensions built by rotten
} rotten_window_wayland;

/**
 * @brief Load the functions from the libwayland-client.so automastically
 * @param lib Pointer to the dispatch table to hold all the function pointers
 * @returns Success code
 * @note I don't think we can fetch the static inline functions for you, instead it's probably easier to use
 * rotten_library_wayland_ext to load all of the wayland functions including the protocol extensions
 */
rotten_success_code rotten_library_wayland_load(rotten_library_wayland* lib);

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
    rotten_dynamic_library* lib;
    rotten_wl_core_dispatch_load_fp lib_load;

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

    // Pointer to a const structs which are exported const symbols from the wayland library, we instead
    // fetch pointers to them. Using the same method as function pointers
    struct wl_interface* registry_interface;
    struct wl_interface* compositor_interface;
    struct wl_interface* surface_interface;
    const struct wl_interface* shm_interface;

} rotten_library_wayland;

ROTTEN_CPP_GUARD_END
#endif