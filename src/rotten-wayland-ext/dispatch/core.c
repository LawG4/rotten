/************************************************************************************************************
 * @file Rotten wayland core loader
 * @brief Loads all of the function pointers for wayland core protocol into a dispatch table
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 ************************************************************************************************************/
#include "../rotten-wayland-ext-internal.h"

void rotten_wl_core_dispatch_load(rotten_library_wayland* lib)
{
    // Give the function pointers for the core library

    // Function pointers for the registries and the raw proxy handles
    lib->registry_interface = &wl_registry_interface;
    lib->proxy_get_version = wl_proxy_get_version;
    lib->proxy_add_listener = wl_proxy_add_listener;
    lib->proxy_marshal_flags = wl_proxy_marshal_flags;

    // Compositor function pointers and interface
    lib->compositor_interface = &wl_compositor_interface;
    lib->compositor_create_surface = wl_compositor_create_surface;

    // Display function pointers
    lib->display_connect = wl_display_connect;
    lib->display_disconnect = wl_display_disconnect;
    lib->display_roundtrip = wl_display_roundtrip;
    lib->display_dispatch = wl_display_dispatch;

    // Surface
    lib->surface_interface = &wl_surface_interface;

    // shared memory function pointers
    lib->shm_interface = &wl_shm_interface;
}