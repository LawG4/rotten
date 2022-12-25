/************************************************************************************************************
 * @file Rotten window wayland extensions header
 * @brief Provides functionality and helper functions for protocols which aren't yet in the core protocol,
 * also provides some extra helpers which may be useful during regular wayland operation
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 *
 * @note Unlike rotten-wayland this library links directly to libwayland-client, it needs to inorder to
 * build the extra protocols. This means linking to rotten-wayland-ext will cause a runtime error if the user
 * doesn't have wayland installed. The workaround is to have a function inside rotten-wayland which loads all
 * the symbols from this project dynamically.
 *
 * This results in the following control flow. directly link to rotten-wayland -> use rotten wayland to
 * determine wayland support at runtime -> dynamically load wayland symbols using rotten-wayland
 * -> dynamically load rotten-wayland-ext symbols using rotten-wayland.
 ************************************************************************************************************/
#ifndef __ROTTEN_EXTERNAL_WINDOW_WAYLAND_EXT_H__
#define __ROTTEN_EXTERNAL_WINDOW_WAYLAND_EXT_H__ (1)
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include "rotten-core.h"
#include "rotten-dynamic-loading.h"
#include "rotten-window.h"
ROTTEN_CPP_GUARD

/**
 * @brief Dispatch table for all of the function pointers we need to load from libwayland-client at runtime
 * @note This library is pretty big, so find the definition at the bottom of rotten-wayland.h
 * When the user is using the rotten-wayland-ext library they only need to fetch one function pointer to fill
 * this struct
 */
typedef struct rotten_library_wayland rotten_library_wayland;

/**
 * @brief Dispatch table for the extensions to the wayland compositor supplied by rotten and usually
 * dynamically loaded.
 * @note Like the core wayland library this table can get rather large, see the bottom of rotten-wayland-ext.h
 * for the definition
 */
typedef struct rotten_library_wayland_ext rotten_library_wayland_ext;

/**
 * @brief Represents the state required to allow all of the extension protocols to function
 */
typedef struct rotten_window_wayland_ext_state rotten_window_wayland_ext_state;

/**
 * @brief Fills in a rotten wayland library handle with function pointers. This is a utility function to allow
 * the user to only need to load one function from rotten-wayland-ext instead of many many functions from
 * libwayland-client. We can do this from rotten-wayland-ext as it is forced to link to wayland itself and can
 * pass on those symbols for you
 * @param lib Pointer to the rotten-library-wayland structure to fill
 */
void rotten_wl_core_dispatch_load(rotten_library_wayland* lib);
typedef void (*rotten_wl_core_dispatch_load_fp)(rotten_library_wayland* lib);

/**
 * @brief Fills in the rotten-library-wayland-ext dispatch table in a similar way as to how the core protocol
 * is loaded
 * @param lib Pointer to the rotten-library-wayland-ext structure to fill
 */
void rotten_wl_ext_dispatch_load(rotten_library_wayland_ext* lib);
typedef void (*rotten_wl_ext_dispatch_load_fp)(rotten_library_wayland_ext* lib);

//
// Provide all the structure name definitions used throughout rotten wayland ext
// This is just the names of the structs so they are incomplete types and basically just fancy void pointers
//
struct xdg_wm_base_listener;
struct xdg_toplevel;
struct xdg_wm_base;
struct xdg_surface_listener;
struct xdg_toplevel_listener;

/**
 * @brief Collect all of the useful information which is used for shared memory
 */
typedef struct rotten_wl_shm_pool {
    struct wl_shm_pool* pool;
    size_t pool_size;
    int file_descriptor;
    uint8_t* ptr;
} rotten_wl_shm_pool;

rotten_wl_shm_pool rotten_wl_shm_create_pool(struct wl_shm* shm, uint32_t size);
void rotten_wl_shm_destroy_pool(rotten_wl_shm_pool* pool);

//
// Provide definitions for the forward declared structures up abpve
//

typedef struct rotten_window_wayland_ext_state {
    struct xdg_wm_base* wm_base;        // Proxy handle to the window manager roles
    struct wl_surface* surface;         // A rectangle which we can display contents to
    struct xdg_surface* xdg_surface;    // A derivative object for the surface to comunicate with wm
    struct xdg_toplevel* xdg_toplevel;  // wm top level roll
} rotten_window_wayland_ext_state;

typedef struct rotten_library_wayland_ext {
    // Native linux handle for lib librotten-wayland-xdg.so
    rotten_dynamic_library* lib;
    rotten_wl_ext_dispatch_load_fp lib_load;

    // xdg interface devices
    const struct wl_interface* xdg_wm_base_interface;
    const struct wl_interface* xdg_surface_interface;
    const struct wl_interface* xdg_toplevel_interface;
    const struct xdg_wm_base_listener* xdg_wm_base_listener;
    const struct xdg_surface_listener* xdg_surface_listener;
    const struct xdg_toplevel_listener* xdg_toplevel_listener;

    // xdg function pointers
    struct xdg_surface* (*xdg_wm_base_get_xdg_surface)(struct xdg_wm_base* xdg_wm_base,
                                                       struct wl_surface* surface);
    struct xdg_toplevel* (*xdg_surface_get_toplevel)(struct xdg_surface* surface);
    void (*xdg_toplevel_set_title)(struct xdg_toplevel* xdg_toplevel, const char* title);
    void (*xdg_wm_base_pong)(struct xdg_wm_base* xdg_wm_base, uint32_t serial);
    int (*xdg_wm_base_add_listener)(struct xdg_wm_base* xdg_wm_base,
                                    const struct xdg_wm_base_listener* listener, void* data);
    int (*xdg_surface_add_listener)(struct xdg_surface* xdg_surface,
                                    const struct xdg_surface_listener* listener, void* data);
    int (*xdg_toplevel_add_listener)(struct xdg_toplevel* xdg_toplevel,
                                     const struct xdg_toplevel_listener* listener, void* data);
} rotten_library_wayland_ext;

ROTTEN_CPP_GUARD_END
#endif